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
#include "Plane3d.h"
#include "Track3dInfo.h"
#include "Video/Frame.h"
#include "Video/VideoSource.h"
#include <QTimer>
#include <math.h>

Track3dController::Track3dController(QObject* parent)
    : QObject(parent)
    , _fps(0)
    , _framesCounter(0)
    , _refreshTextCounter(0)
    , _videoSource(nullptr)
    , _frame(nullptr)
    , _aruco(nullptr)
    , _imageInvalidated(false)
    , _textInvalidated(false)
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
    return _image;
}

void Track3dController::setFrame(Frame* f)
{
    if (_frame == f)
        return;

    _frame = f;
    _imageInvalidated = true;
    _textInvalidated = true;
}

void Track3dController::setVideoSource(VideoSource* videoSource)
{
    if (_videoSource == videoSource)
        return;

    if (_videoSource) {
        disconnect(_videoSource, 0, this, 0);
    }

    _videoSource = videoSource;

    if (_videoSource) {
        connect(_videoSource, &VideoSource::frameChanged, this, &Track3dController::setFrame);
    }
    emit videoSourceChanged(_videoSource);
}

void Track3dController::setAruco(Aruco* aruco)
{
    if (_aruco == aruco)
        return;

    _aruco = aruco;
    emit arucoChanged(_aruco);
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
    if (_imageInvalidated) {
        _image = _frame ? _frame->image() : QImage();

        if (_aruco) {
            _markers = _aruco->detectMarkers(_image);
            _aruco->drawMarkers(_image, _markers);
        }
        _framesCounter++;
        if (_elapsedTime.elapsed() > 500) {
            updateFps();
        }

        _imageInvalidated = false;
        emit imageChanged();

        if (++_refreshTextCounter == 15) {
            refreshText();
            _refreshTextCounter = 0;
        }
    }
}

void Track3dController::refreshText()
{
    QList<QVector3D> points;
    QSet<int> foundIds;
    bool newIdAdded = false;
    for (size_t i = 0; i < _markers.ids.size(); ++i) {
        const int id = _markers.ids.at(i);
        foundIds << id;

        if (!_markerInfos.contains(id)) {
            newIdAdded = true;
            _markerInfos[id] = new Track3dInfo(id, this);
        }
        auto tvec = _markers.tvecs.at(i);
        auto rvec = _markers.rvecs.at(i);
        _markerInfos[id]->setPositionRotation(tvec[0], tvec[1], tvec[2], rvec[0], rvec[1], rvec[2]);
        points << QVector3D(tvec[0], tvec[1], tvec[2]);
    }
    auto missingIds = _markerInfos.keys().toSet() - foundIds;
    for (auto id : missingIds) {
        _markerInfos[id]->setNotDetected();
    }

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

VideoSource* Track3dController::videoSource() const
{
    return _videoSource;
}

Aruco* Track3dController::aruco() const
{
    return _aruco;
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
