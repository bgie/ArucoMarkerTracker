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
#include <Aruco/Aruco.h>
#include <QElapsedTimer>
#include <QImage>
#include <QMap>
#include <QObject>

class VideoSource;
class Frame;
class Track3dInfo;

class Track3dController : public QObject {
    Q_OBJECT

    Q_PROPERTY(VideoSource* videoSource READ videoSource WRITE setVideoSource NOTIFY videoSourceChanged)
    Q_PROPERTY(Aruco* aruco READ aruco WRITE setAruco NOTIFY arucoChanged)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(qreal fps READ fps NOTIFY fpsChanged)
    Q_PROPERTY(QList<QObject*> markers READ markerQObjects NOTIFY markersChanged);
    Q_PROPERTY(QString refPlane READ refPlane WRITE setRefPlane NOTIFY refPlaneChanged)

public:
    explicit Track3dController(QObject* parent = nullptr);
    virtual ~Track3dController();

    QImage image();
    qreal fps() const;
    QList<Track3dInfo*> markers() const;
    QList<QObject*> markerQObjects() const;
    QString refPlane() const;

    VideoSource* videoSource() const;
    Aruco* aruco() const;

signals:
    void imageChanged();
    void fpsChanged(qreal fps);
    void markersChanged();
    void refPlaneChanged(QString refPlane);

    void videoSourceChanged(VideoSource* videoSource);
    void arucoChanged(Aruco* aruco);

public slots:
    void setVideoSource(VideoSource* videoSource);
    void setAruco(Aruco* aruco);

private slots:
    void setRefPlane(QString refPlane);
    void setFrame(Frame* f);
    void setFps(qreal fps);
    void updateFps();
    void refreshImage();
    void refreshText();

private:
    QImage _image;
    qreal _fps;
    int _framesCounter;
    QElapsedTimer _elapsedTime;
    QTimer* _refreshImageTimer;
    int _refreshTextCounter;
    QTimer* _refreshFpsTimer;
    VideoSource* _videoSource;
    Frame* _frame;
    Aruco* _aruco;
    bool _imageInvalidated;
    bool _textInvalidated;
    Aruco::Markers _markers;
    std::vector<float> _angles;
    QMap<int, Track3dInfo*> _markerInfos;
    QString _refPlane;
};
