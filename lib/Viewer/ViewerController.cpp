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
#include "ViewerController.h"
#include "Video/Frame.h"
#include "Video/VideoSource.h"
#include <QTimer>

ViewerController::ViewerController(QObject* parent)
    : QObject(parent)
    , _frame(nullptr)
    , _fps(0)
    , _framesCounter(0)
    , _videoSource(nullptr)
{
    _elapsedTime.start();
    _refreshFpsTimer = new QTimer(this);
    QObject::connect(_refreshFpsTimer, &QTimer::timeout, this, &ViewerController::updateFps);
    _refreshFpsTimer->setInterval(1000);
    _refreshFpsTimer->setSingleShot(false);
    _refreshFpsTimer->start();
}

ViewerController::~ViewerController()
{
}

QImage ViewerController::image() const
{
    return _image;
}

void ViewerController::setFrame(Frame* frame)
{
    if (_frame == frame)
        return;

    _frame = frame;
    QImage newImage = _frame ? _frame->image() : QImage();

    if (_image == newImage)
        return;

    _image = newImage;
    _framesCounter++;
    emit imageChanged(_image);

    if (_elapsedTime.elapsed() > 500) {
        updateFps();
    }
}

void ViewerController::setVideoSource(VideoSource* videoSource)
{
    if (_videoSource == videoSource)
        return;

    if (_videoSource) {
        disconnect(_videoSource, 0, this, 0);
    }

    _videoSource = videoSource;

    if (_videoSource) {
        connect(_videoSource, &VideoSource::frameChanged, this, &ViewerController::setFrame);
    }
    emit videoSourceChanged(_videoSource);
}

void ViewerController::setFps(qreal fps)
{
    if (qFuzzyCompare(_fps, fps))
        return;

    _fps = fps;
    emit fpsChanged(_fps);
}

void ViewerController::updateFps()
{
    auto elapsed = _elapsedTime.restart();
    qreal newfps = _framesCounter * 1000.0 / elapsed;
    _framesCounter = 0;
    setFps(newfps);
    _refreshFpsTimer->start();
}

qreal ViewerController::fps() const
{
    return _fps;
}

VideoSource* ViewerController::videoSource() const
{
    return _videoSource;
}
