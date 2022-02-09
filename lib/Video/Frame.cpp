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
#include "Frame.h"
#include <QtConcurrent/QtConcurrentRun>

Frame::Frame(QDir path, QString fileName, QObject* parent)
    : QObject(parent)
    , _path(path)
    , _fileName(fileName)
    , _needsLoadFromDisk(true)
{
}

Frame::Frame(QImage image, QObject* parent)
    : QObject(parent)
    , _image(image)
    , _needsLoadFromDisk(false)
{
}

Frame::~Frame()
{
}

QString Frame::fileName() const
{
    return _fileName;
}

QImage Frame::image()
{
    if (_needsLoadFromDisk) {
        loadImageFromDisk();
        _image = _imageFuture.result();
    }
    return _image;
}

void Frame::loadImageFromDisk()
{
    if (_needsLoadFromDisk && _imageFuture.isCanceled()) {
        QString fullFileName = _path.absoluteFilePath(_fileName);
        _imageFuture = QtConcurrent::run([fullFileName]() -> QImage {
            return QImage(fullFileName).convertToFormat(QImage::Format_RGB888);
        });
    }
}
