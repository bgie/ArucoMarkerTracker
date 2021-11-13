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
#include "Camera.h"
#include "CameraReader.h"
#include <QDebug>
#include <QFile>
#include <QList>
#include <QPair>
#include <QSize>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>

const int EXPOSURE_CTRLID = 10094850;

QMap<uint32_t, int32_t> defaultCameraSettings = {
    { 9963776, 128 }, //  Brightness
    { 9963777, 128 }, //  Contrast
    { 9963778, 128 }, //  Saturation
    { 9963788, 1 }, //  White Balance Temperature, Auto
    { 9963795, 255 }, //  Gain
    { 9963800, 1 }, //  Power Line Frequency
    //{ 9963802, 3500 }, //  White Balance Temperature
    { 9963803, 128 }, //  Sharpness
    { 9963804, 1 }, //  Backlight Compensation
    { 10094849, 1 }, //  Exposure, Auto
    //{ EXPOSURE_CTRLID, 80 }, //  Exposure (Absolute)
    { 10094851, 0 }, //  Exposure, Auto Priority
    { 10094856, 0 }, //  Pan (Absolute)
    { 10094857, 0 }, //  Tilt (Absolute)
    { 10094858, 0 }, //  Focus (absolute)
    { 10094860, 0 }, //  Focus, Auto
    { 10094861, 100 } //  Zoom, Absolute
};

struct VideoFormat {
    QString description;
    uint32_t format;
    QSize size;
    QPair<uint32_t, uint32_t> fps;
};

struct CameraData {
public:
    CameraData(QString deviceName)
        : deviceName(deviceName)
        , fd(0)
        , videoFormatIndex(-1)
        , exposure(100)
    {
    }

    QString deviceName;
    int fd;
    QList<VideoFormat> videoFormats;
    int videoFormatIndex;
    int exposure;
    QScopedPointer<CameraReader> reader;
};

Camera::Camera(QString deviceName, QObject* parent)
    : QObject(parent)
    , _d(new CameraData(deviceName))
{
    _d->fd = open(deviceName.toLatin1().constData(), O_RDWR);
    if (_d->fd < 0) {
        qCritical() << "Error opening camera, errno: " << errno;
    }

    // stream formats loop
    struct v4l2_fmtdesc format;
    memset(&format, 0, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while (0 == xioctl(_d->fd, VIDIOC_ENUM_FMT, &format)) {
        if (format.pixelformat == V4L2_PIX_FMT_MJPEG) {

            // pixel formats loop
            struct v4l2_frmsizeenum size;
            memset(&size, 0, sizeof(size));
            size.pixel_format = format.pixelformat;
            while (0 == xioctl(_d->fd, VIDIOC_ENUM_FRAMESIZES, &size)) {
                if (size.type == V4L2_FRMSIZE_TYPE_DISCRETE) {

                    // frame rates loop
                    struct v4l2_frmivalenum fps;
                    memset(&fps, 0, sizeof(fps));
                    fps.pixel_format = format.pixelformat;
                    fps.height = size.discrete.height;
                    fps.width = size.discrete.width;
                    while (0 == xioctl(_d->fd, VIDIOC_ENUM_FRAMEINTERVALS, &fps)) {

                        VideoFormat f;
                        f.format = format.pixelformat;
                        f.size = QSize(size.discrete.width, size.discrete.height);
                        f.fps = QPair<uint32_t, uint32_t>(fps.discrete.denominator, fps.discrete.numerator);
                        f.description = QStringLiteral("%1 - %2x%3 - %4 fps")
                                            .arg(QString::fromLatin1(reinterpret_cast<const char*>(format.description)))
                                            .arg(f.size.width())
                                            .arg(f.size.height())
                                            .arg(qreal(f.fps.first) / f.fps.second, 0, 'f', 2);
                        _d->videoFormats.append(f);
                        fps.index++;
                    }
                }
                size.index++;
            }
        }
        format.index++;
    }
}

Camera::~Camera()
{
    stopStream();
    close(_d->fd);
}

QString Camera::deviceName() const
{
    return _d->deviceName;
}

QStringList Camera::videoFormats() const
{
    QStringList result;
    for (const VideoFormat& format : _d->videoFormats) {
        result.append(format.description);
    }
    return result;
}

void Camera::setVideoFormatIndex(int i)
{
    _d->videoFormatIndex = i;
}

void Camera::setExposure(int val)
{
    _d->exposure = val;

    if (_d->reader) {
        updateExposure();
    }
}

bool Camera::canStream() const
{
    return _d->videoFormatIndex >= 0 && _d->videoFormatIndex < _d->videoFormats.count();
}

void Camera::startStream()
{
    if (_d->reader.isNull() && canStream()) {
        const VideoFormat& format = _d->videoFormats.at(_d->videoFormatIndex);
        struct v4l2_format fmt;
        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = format.size.width();
        fmt.fmt.pix.height = format.size.height();
        fmt.fmt.pix.pixelformat = format.format;
        fmt.fmt.pix.field = V4L2_FIELD_NONE;
        if (-1 == xioctl(_d->fd, VIDIOC_S_FMT, &fmt)) {
            qCritical() << "Cannot set video format";
        }

        struct v4l2_streamparm fps;
        memset(&fps, 0, sizeof(fps));
        fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        qDebug() << "hebbes" << format.fps.first << " " << format.fps.second;
        fps.parm.capture.timeperframe.denominator = format.fps.first;
        fps.parm.capture.timeperframe.numerator = format.fps.second;
        if (-1 == xioctl(_d->fd, VIDIOC_S_PARM, &fps)) {
            qCritical() << "Cannot set framerate";
        }

        struct v4l2_queryctrl qctrl;
        memset(&qctrl, 0, sizeof(qctrl));
        qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
        QMap<uint32_t, QByteArray> controls;
        while (0 == ioctl(_d->fd, VIDIOC_QUERYCTRL, &qctrl)) {
            controls[qctrl.id] = QByteArray((const char*)qctrl.name);
            qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
        }

        struct v4l2_control ctrl;
        for (uint32_t id : controls.keys()) {
            memset(&ctrl, 0, sizeof(ctrl));
            ctrl.id = id;
            if (-1 == xioctl(_d->fd, VIDIOC_G_CTRL, &ctrl)) {
                qCritical() << "Cannot get control value";
            }
            qDebug() << "{" << id << ", " << ctrl.value << "}, // " << qPrintable(controls[id]);
        }

        for (uint32_t id : controls.keys()) {
            if (defaultCameraSettings.contains(id)) {
                memset(&ctrl, 0, sizeof(ctrl));
                ctrl.id = id;
                ctrl.value = defaultCameraSettings[id];
                if (-1 == xioctl(_d->fd, VIDIOC_S_CTRL, &ctrl)) {
                    qCritical() << "Cannot set control value " << qPrintable(controls[id]);
                }
            }
        }

        updateExposure();

        _d->reader.reset(new CameraReader(_d->fd, format.size));
        connect(_d->reader.data(), &CameraReader::frameRead, this, &Camera::frameReceived, Qt::QueuedConnection);
    }
}

void Camera::stopStream()
{
    if (!_d->reader.isNull()) {
        _d->reader.reset();
    }
}

bool Camera::isValidDevice(QString deviceName)
{
    return QFile::exists(deviceName);
}

void Camera::frameReceived(const QImage img)
{
    emit frameRead(img);
}

void Camera::updateExposure()
{
    struct v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = EXPOSURE_CTRLID;
    ctrl.value = _d->exposure;
    if (-1 == xioctl(_d->fd, VIDIOC_S_CTRL, &ctrl)) {
        qCritical() << "Cannot set exposure value";
    }
}
