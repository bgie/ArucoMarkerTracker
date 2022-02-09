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
#include "Track3dController.h"
#include "Aruco/Aruco.h"
#include "Marker.h"
#include "Plane3d.h"
#include "Track3d/ObjectTracker.h"
#include "Track3dInfo.h"
#include <QMutexLocker>
#include <QTimer>
#include <math.h>

Track3dController::Track3dController(QObject* parent)
    : QObject(parent)
    , _fps(0)
    , _framesCounter(0)
    , _refreshTextCounter(0)
    , _objectTracker(nullptr)
    , _aruco(nullptr)
{
    _elapsedTime.start();
    _refreshFpsTimer = new QTimer(this);

    QObject::connect(_refreshFpsTimer, &QTimer::timeout, this, &Track3dController::updateFps);
    _refreshFpsTimer->setInterval(1000);
    _refreshFpsTimer->setSingleShot(false);
    _refreshFpsTimer->start();

    _refreshImageTimer = new QTimer(this);
    QObject::connect(_refreshImageTimer, &QTimer::timeout, this, &Track3dController::refreshImage);
    _refreshImageTimer->setInterval(1000 / 60);
    _refreshImageTimer->setSingleShot(false);
    _refreshImageTimer->start();
}

Track3dController::~Track3dController()
{
}

QImage Track3dController::image()
{
    return _annotatedImage;
}

void Track3dController::setAruco(Aruco* aruco)
{
    _aruco = aruco;
}

void Track3dController::setFps(qreal fps)
{
    if (qFuzzyCompare(_fps, fps))
        return;

    _fps = fps;
    emit fpsChanged(_fps);
}

void Track3dController::updateFps()
{
    auto elapsed = _elapsedTime.restart();
    qreal newfps = _framesCounter * 1000.0 / elapsed;
    _framesCounter = 0;
    setFps(newfps);
    _refreshFpsTimer->start();
}

void Track3dController::refreshImage()
{
    if (!_objectTracker)
        return;

    QMutexLocker lock(_objectTracker->mutex());
    QImage newImage = _objectTracker->image();

    if (_image != newImage) {
        _image = newImage;
        _annotatedImage = newImage;

        if (_aruco) {
            auto markers = _objectTracker->markers();
            lock.unlock();
            _aruco->drawMarkers(_annotatedImage, markers);
        } else {
            lock.unlock();
        }

        _framesCounter++;
        if (_elapsedTime.elapsed() > 500) {
            updateFps();
        }

        emit imageChanged();

        if (++_refreshTextCounter == 15) {
            refreshText();
            _refreshTextCounter = 0;
        }
    }
}

void Track3dController::refreshText()
{
    if (!_objectTracker)
        return;

    QMutexLocker lock(_objectTracker->mutex());
    auto idToMarker = _objectTracker->idToMarker();
    bool newIdAdded = false;
    QList<QVector3D> points;
    for (auto it = idToMarker.cbegin(); it != idToMarker.cend(); ++it) {
        if (!_markerInfos.contains(it.key())) {
            newIdAdded = true;
            _markerInfos[it.key()] = new Track3dInfo(it.key(), this);
        }
        _markerInfos[it.key()]->update(it.value());
        points << it.value()->filteredPos();
    }
    lock.unlock();

    bool hasPlane = false;
    auto plane = Plane3d::fitToPoints(points, &hasPlane);
    if (hasPlane) {
        setRefPlane(
            QStringLiteral("xAngle=%1 yAngle=%2 zAngle=%3")
                .arg(180 * plane.xAngle() / M_PI, 0, 'f', 2)
                .arg(180 * plane.yAngle() / M_PI, 0, 'f', 2)
                .arg(180 * plane.zAngle() / M_PI, 0, 'f', 2));
    } else {
        setRefPlane(QStringLiteral("-"));
    }

    if (newIdAdded) {
        emit markersChanged();
    }
}

qreal Track3dController::fps() const
{
    return _fps;
}

QList<Track3dInfo*> Track3dController::markers() const
{
    return _markerInfos.values();
}

QList<QObject*> Track3dController::markerQObjects() const
{
    QList<QObject*> result;
    foreach (auto m, _markerInfos.values()) {
        result << m;
    }
    return result;
}

Aruco* Track3dController::aruco() const
{
    return _aruco;
}

void Track3dController::setObjectTracker(ObjectTracker* objectTracker)
{
    _objectTracker = objectTracker;
}

void Track3dController::setRefPlane(QString refPlane)
{
    if (_refPlane == refPlane)
        return;

    _refPlane = refPlane;
    emit refPlaneChanged(_refPlane);
}

QString Track3dController::refPlane() const
{
    return _refPlane;
}

ObjectTracker* Track3dController::objectTracker() const
{
    return _objectTracker;
}
