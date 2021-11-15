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
#include <QJsonArray>
#include <QtConcurrent/QtConcurrentRun>

namespace {
const QString MARKERS(QStringLiteral("markers"));
const QString FILENAME(QStringLiteral("filename"));
}

Frame::Frame(QDir path, QString fileName, QObject* parent)
    : QObject(parent)
    , _path(path)
    , _fileName(fileName)
{
    loadImage();
}

Frame::Frame(QDir path, QJsonObject obj, QObject* parent)
    : QObject(parent)
    , _path(path)
    , _fileName(obj[FILENAME].toString())
{
    QJsonArray arr = obj[MARKERS].toArray();
    for (auto it = arr.constBegin(); it != arr.constEnd(); ++it) {
        _markers.append(new Marker((*it).toObject()));
    }
}

Frame::~Frame()
{
    qDeleteAll(_markers);
}

QString Frame::fileName() const
{
    return _fileName;
}

QImage Frame::image()
{
    loadImage();
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

QJsonObject Frame::toJson() const
{
    QJsonObject obj;
    QJsonArray arr;
    for (auto m : _markers) {
        arr.append(m->toJson());
    }
    obj.insert(MARKERS, arr);
    obj.insert(FILENAME, _fileName);
    return obj;
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
    if (_markers != list) {
        qDeleteAll(_markers.toSet() - list.toSet());
        _markers = list;
        emit markersChanged();
    }
}

void Frame::loadImage()
{
    if (_image.isCanceled()) {
        _image = QtConcurrent::run([=]() -> QImage {
            return QImage(_path.absoluteFilePath(_fileName)).convertToFormat(QImage::Format_RGB888);
        });
    }
}
