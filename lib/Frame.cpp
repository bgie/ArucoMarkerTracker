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

Frame::Frame(QString fileName, QString fullFileName, QObject* parent)
    : QObject(parent)
    , _fileName(fileName)
{
    _image = QtConcurrent::run([=]() -> QImage {
        return QImage(fullFileName).convertToFormat(QImage::Format_RGB888);
    });
}

Frame::~Frame()
{
}

QString Frame::fileName() const
{
    return _fileName;
}

QImage Frame::image() const
{
    return _image.result();
}

QString Frame::chessBoardReprojectionError() const
{
    return _chessBoardReprojectionError;
}

QList<QObject*> Frame::markersQObjects() const
{
    QList<QObject*> result;
    for (auto m : _markers) {
        result.append(m);
    }
    return result;
}

QList<Marker*> Frame::markers() const
{
    return _markers;
}

void Frame::setChessBoardReprojectionError(QString chessBoardReprojectionError)
{
    if (_chessBoardReprojectionError == chessBoardReprojectionError)
        return;

    _chessBoardReprojectionError = chessBoardReprojectionError;
    emit chessBoardReprojectionErrorChanged(_chessBoardReprojectionError);
}

void Frame::setMarkers(QList<Marker*> list)
{
    _markers = list;
    emit markersChanged();
}
