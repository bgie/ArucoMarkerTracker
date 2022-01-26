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
#include "ReplayController.h"
#include "Video/Frame.h"
#include "Video/Video.h"
#include "Video/VideoSource.h"
#include <QDir>
#include <QSettings>

namespace {
const int PREFETCH_FRAME_COUNT = 50;
const QString LOADPATH_KEY(QStringLiteral("LoadPath"));
const QString REPLAYFPS_KEY(QStringLiteral("ReplayFps"));
}

ReplayController::ReplayController(QObject* parent)
    : QObject(parent)
    , _video(new Video(this))
    , _canReplayStream(false)
    , _isReplayStreaming(false)
    , _frameIndex(-1)
    , _videoSource(nullptr)
{
    QSettings settings;
    setLoadPath(settings.value(LOADPATH_KEY, QDir::homePath()).toString());
    setReplayFps(settings.value(REPLAYFPS_KEY, QStringLiteral("10")).toString());

    QObject::connect(&_replayTimer, &ReplayTimer::playFrame, this, &ReplayController::setFrameIndex);

    setFrameIndex(-1);
}

ReplayController::~ReplayController()
{
}

void ReplayController::setFrameIndex(int index)
{
    if (_frameIndex != index) {
        _frameIndex = index;

        Frame* frame = nullptr;
        if (_frameIndex >= 0 && _frameIndex < _video->frames().length()) {
            frame = _video->frames().at(index);
        }
        _image = frame ? frame->image() : QImage();
        emit frameIndexChanged(_frameIndex);
        emit imageChanged(_image);

        if (_videoSource) {
            _videoSource->setFrame(frame);
        }

        while (!_indicesToPrefetch.isEmpty() && (index = _indicesToPrefetch.last()) < _frameIndex + PREFETCH_FRAME_COUNT) {
            _video->frames().at(index)->loadImageFromDisk();
            _indicesToPrefetch.pop_back();
        }
    }
}

void ReplayController::setLoadPath(QString loadPath)
{
    if (_loadPath == loadPath)
        return;

    _loadPath = loadPath;

    QSettings settings;
    settings.setValue(LOADPATH_KEY, _loadPath);

    _video->load(_loadPath);
    _indicesToPrefetch.clear();
    const int prefetch = qMin(_video->frames().size(), PREFETCH_FRAME_COUNT);
    for (int i = 0; i < prefetch; ++i) {
        _video->frames().at(i)->loadImageFromDisk();
    }
    for (int i = _video->frames().count() - 1; i >= prefetch; --i) {
        _indicesToPrefetch.push_back(i);
    }
    _replayTimer.setFrameCount(_video->frames().count());
    setCanReplayStream(_replayTimer.canReplay());
    setFrameIndex(-1);

    emit loadPathChanged(_loadPath);
}

void ReplayController::setReplayFps(QString replayFps)
{
    if (_replayFps == replayFps)
        return;

    _replayFps = replayFps;

    QSettings settings;
    settings.setValue(REPLAYFPS_KEY, _replayFps);

    _replayTimer.setFps(_replayFps.toInt());
    setCanReplayStream(_replayTimer.canReplay());

    emit replayFpsChanged(_replayFps);
}

void ReplayController::setVideoSource(VideoSource* videoSource)
{
    if (_videoSource == videoSource)
        return;

    _videoSource = videoSource;
    emit videoSourceChanged(_videoSource);
}

void ReplayController::setCanReplayStream(bool canReplayStream)
{
    if (_canReplayStream == canReplayStream)
        return;

    _canReplayStream = canReplayStream;
    emit canReplayStreamChanged(_canReplayStream);
}

void ReplayController::setIsReplayStreaming(bool isReplayStreaming)
{
    if (_isReplayStreaming == isReplayStreaming)
        return;

    _isReplayStreaming = isReplayStreaming;
    emit isReplayStreamingChanged(_isReplayStreaming);
}

QString ReplayController::loadPath() const
{
    return _loadPath;
}

QString ReplayController::replayFps() const
{
    return _replayFps;
}

bool ReplayController::canReplayStream() const
{
    return _canReplayStream;
}

void ReplayController::startReplayStream()
{
    if (_replayTimer.canReplay() && !_isReplayStreaming) {
        if (_videoSource) {
            _videoSource->setExclusiveFrameProvider([=]() { this->stopReplayStream(); });
        }

        _replayTimer.setAutoPlayEnabled(true);
        setIsReplayStreaming(true);
    }
}

void ReplayController::stopReplayStream()
{
    if (_isReplayStreaming) {
        _replayTimer.setAutoPlayEnabled(false);
        setIsReplayStreaming(false);
    }
}

bool ReplayController::isReplayStreaming() const
{
    return _isReplayStreaming;
}

QImage ReplayController::image() const
{
    return _image;
}

int ReplayController::frameIndex() const
{
    return _frameIndex;
}

VideoSource* ReplayController::videoSource() const
{
    return _videoSource;
}
