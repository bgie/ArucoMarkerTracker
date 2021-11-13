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
#include "ReplayTimer.h"
#include <math.h>

ReplayTimer::ReplayTimer(QObject* parent)
    : QObject(parent)
    , _frameCount(0)
    , _fps(0)
    , _autoPlay(false)
    , _sentFrames(0)
    , _lastFrameIndex(0)
{
    _timer.setInterval(0);
    _timer.setSingleShot(false);
    connect(&_timer, &QTimer::timeout, this, &ReplayTimer::timeout);
}

void ReplayTimer::setFrameCount(int count)
{
    _frameCount = count;
    _lastFrameIndex = -1;
}

void ReplayTimer::setFps(qreal fps)
{
    _fps = fps;
}

bool ReplayTimer::canReplay() const
{
    return _frameCount > 0 && _fps > 0;
}

void ReplayTimer::setAutoPlayEnabled(bool enabled)
{
    if (_autoPlay == enabled)
        return;

    _autoPlay = enabled;

    if (_autoPlay) {
        _elapsedTime.start();
        _sentFrames = 0;
        _timer.start();
    } else {
        _timer.stop();
    }
}

void ReplayTimer::timeout()
{
    if (!canReplay())
        return;

    qint64 elapsed = _elapsedTime.elapsed();
    qint64 shouldHaveSent = static_cast<qint64>(floor(elapsed * _fps / 1000));
    while (_sentFrames < shouldHaveSent) {
        _lastFrameIndex = (_lastFrameIndex + 1) % _frameCount;
        emit playFrame(_lastFrameIndex);

        _sentFrames++;
    }
}
