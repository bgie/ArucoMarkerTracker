/*  ArucoMarkerTracker
    Copyright (C) 2021 Kuppens Brecht
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "CameraReader.h"
#include <QDebug>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <unistd.h>

int xioctl(int fd, int request, void* arg)
{
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}

CameraReader::CameraReader(int fd, QSize frameSize)
    : _fd(fd)
    , _frameSize(frameSize)
    , _stopReading(false)
{
    this->start(QThread::TimeCriticalPriority);
}

CameraReader::~CameraReader()
{
    _stopReading = true;
    this->wait();
}

void CameraReader::run()
{
    qDebug() << "CameraReader run started";
    init();

    while (!_stopReading) {
        if (!readFrame()) {
            this->yieldCurrentThread();
        }
    }

    clear();

    qDebug() << "CameraReader run finished";
}

void CameraReader::init(void)
{
    struct v4l2_requestbuffers reqbuf;
    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    reqbuf.count = 5;
    if (-1 == xioctl(_fd, VIDIOC_REQBUFS, &reqbuf)) {
        qCritical() << "Error requesting camera buffers, errno: " << errno;
    }

    if (reqbuf.count < 2) {
        qCritical() << "Not enough memory for camera buffers";
    }

    _buffers.resize(reqbuf.count);

    // Create the buffer memory maps
    struct v4l2_buffer buffer;
    for (unsigned int i = 0; i < reqbuf.count; i++) {
        memset(&buffer, 0, sizeof(buffer));
        buffer.type = reqbuf.type;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        // Note: VIDIOC_QUERYBUF, not VIDIOC_QBUF, is used here!
        if (-1 == xioctl(_fd, VIDIOC_QUERYBUF, &buffer)) {
            perror("VIDIOC_QUERYBUF");
            exit(errno);
        }

        _buffers[i].length = buffer.length;
        _buffers[i].start = mmap(
            NULL,
            buffer.length,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            _fd,
            buffer.m.offset);

        if (MAP_FAILED == _buffers[i].start) {
            qCritical() << "MMap failed";
        }
    }

    for (int i = 0; i < _buffers.count(); i++) {
        memset(&buffer, 0, sizeof(buffer));
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        // Enqueue the buffer with VIDIOC_QBUF
        if (-1 == xioctl(_fd, VIDIOC_QBUF, &buffer)) {
            qCritical() << "VIDIOC_QBUF error, errno: " << errno;
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(_fd, VIDIOC_STREAMON, &type)) {
        qCritical() << "VIDIOC_STREAMON error, errno: " << errno;
    }
}

bool CameraReader::readFrame()
{
    struct v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;

    // Dequeue a buffer
    if (-1 == xioctl(_fd, VIDIOC_DQBUF, &buffer)) {
        switch (errno) {
        case EAGAIN:
            // No buffer in the outgoing queue
            return false;
        case EIO:
            // fall through
        default:
            qCritical() << "Error dequeing buffer";
            return true;
        }
    }

    QElapsedTimer timer;
    timer.start();

    auto jpgBuffera = reinterpret_cast<const uchar*>(_buffers.at(buffer.index).start);
    QImage img = QImage::fromData(jpgBuffera, _buffers.at(buffer.index).length, "JPG");
    img = img.convertToFormat(QImage::Format_RGB888);

    // Enqueue the buffer again
    if (-1 == xioctl(_fd, VIDIOC_QBUF, &buffer)) {
        qCritical() << "VIDIOC_QBUF error, errno: " << errno;
    }

    emit frameRead(img, timer);
    return true;
}

void CameraReader::clear()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(_fd, VIDIOC_STREAMOFF, &type)) {
        qCritical() << "VIDIOC_STREAMOFF error, errno: " << errno;
    }

    struct v4l2_requestbuffers reqbuf;
    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    reqbuf.count = 0;
    if (-1 == xioctl(_fd, VIDIOC_REQBUFS, &reqbuf)) {
        qCritical() << "Error requesting camera buffers, errno: " << errno;
    }

    for (int i = 0; i < _buffers.count(); i++) {
        munmap(_buffers.at(i).start, _buffers.at(i).length);
    }
    _buffers.clear();
}
