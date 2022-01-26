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
#pragma once
#include <QImage>
#include <QSize>
#include <QThread>
#include <QVector>

int xioctl(int fd, int request, void* arg);

class CameraReader : public QThread {
    Q_OBJECT

public:
    explicit CameraReader(int fd, QSize frameSize);
    virtual ~CameraReader();

signals:
    void frameRead(const QImage img);

protected:
    virtual void run() override;

private:
    void init();
    bool readFrame();
    void clear();

private:
    struct mmapBuffer {
        void* start;
        size_t length;
    };

private:
    const int _fd;
    const QSize _frameSize;
    volatile bool _stopReading;
    QVector<struct mmapBuffer> _buffers;
};
