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
#include <QElapsedTimer>
#include <QImage>
#include <QObject>

class VideoSource;
class Frame;

class ViewerController : public QObject {
    Q_OBJECT

    Q_PROPERTY(VideoSource* videoSource READ videoSource WRITE setVideoSource NOTIFY videoSourceChanged)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(qreal fps READ fps NOTIFY fpsChanged)

public:
    explicit ViewerController(QObject* parent = nullptr);
    virtual ~ViewerController();

    QImage image() const;
    qreal fps() const;

    VideoSource* videoSource() const;

signals:
    void imageChanged(QImage image);
    void fpsChanged(qreal fps);
    void videoSourceChanged(VideoSource* videoSource);

public slots:
    void setFrame(Frame* frame);
    void setVideoSource(VideoSource* videoSource);

private slots:
    void setFps(qreal fps);
    void updateFps();

private:
    Frame* _frame;
    QImage _image;
    qreal _fps;
    int _framesCounter;
    QElapsedTimer _elapsedTime;
    QTimer* _refreshFpsTimer;
    VideoSource* _videoSource;
};
