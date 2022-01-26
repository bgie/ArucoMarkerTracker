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
#include "ReplayTimer.h"
#include <QImage>
#include <QObject>

class Video;
class VideoSource;

class ReplayController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString loadPath READ loadPath WRITE setLoadPath NOTIFY loadPathChanged)
    Q_PROPERTY(QString replayFps READ replayFps WRITE setReplayFps NOTIFY replayFpsChanged)
    Q_PROPERTY(bool canReplayStream READ canReplayStream NOTIFY canReplayStreamChanged)
    Q_PROPERTY(bool isReplayStreaming READ isReplayStreaming NOTIFY isReplayStreamingChanged)
    Q_PROPERTY(int frameIndex READ frameIndex NOTIFY frameIndexChanged)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(VideoSource* videoSource READ videoSource WRITE setVideoSource NOTIFY videoSourceChanged)

public:
    explicit ReplayController(QObject* parent = nullptr);
    virtual ~ReplayController() override;

    QString loadPath() const;
    QString replayFps() const;
    bool canReplayStream() const;
    Q_INVOKABLE void startReplayStream();
    Q_INVOKABLE void stopReplayStream();
    bool isReplayStreaming() const;

    QImage image() const;
    int frameIndex() const;

    VideoSource* videoSource() const;

public slots:
    void setLoadPath(QString loadPath);
    void setReplayFps(QString replayFps);
    void setVideoSource(VideoSource* videoSource);

signals:
    void imageChanged(QImage image);
    void frameIndexChanged(int index);
    void loadPathChanged(QString loadPath);
    void replayFpsChanged(QString replayFps);
    void canReplayStreamChanged(bool canReplayStream);
    void isReplayStreamingChanged(bool isReplayStreaming);
    void videoSourceChanged(VideoSource* videoSource);

private:
    void setFrameIndex(int index);
    void setCanReplayStream(bool canReplayStream);
    void setIsReplayStreaming(bool isReplayStreaming);

private:
    Video* _video;
    QImage _image;
    QString _loadPath;
    QString _replayFps;
    bool _canReplayStream;
    ReplayTimer _replayTimer;
    bool _isReplayStreaming;
    int _frameIndex;
    QVector<int> _indicesToPrefetch;
    VideoSource* _videoSource;
};
