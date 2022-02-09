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

class Track3dInfo;
class ObjectTracker;

class Track3dController : public QObject {
    Q_OBJECT

    Q_PROPERTY(ObjectTracker* objectTracker READ objectTracker WRITE setObjectTracker)
    Q_PROPERTY(Aruco* aruco READ aruco WRITE setAruco)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(qreal fps READ fps NOTIFY fpsChanged)
    Q_PROPERTY(QList<QObject*> markers READ markerQObjects NOTIFY markersChanged);
    Q_PROPERTY(QString refPlane READ refPlane WRITE setRefPlane NOTIFY refPlaneChanged)

public:
    explicit Track3dController(QObject* parent = nullptr);
    virtual ~Track3dController();

    ObjectTracker* objectTracker() const;
    void setObjectTracker(ObjectTracker* objectTracker);
    Aruco* aruco() const;
    void setAruco(Aruco* aruco);

    QImage image();
    qreal fps() const;
    QList<Track3dInfo*> markers() const;
    QList<QObject*> markerQObjects() const;
    QString refPlane() const;

signals:
    void imageChanged();
    void fpsChanged(qreal fps);
    void markersChanged();
    void refPlaneChanged(QString refPlane);

private slots:
    void setRefPlane(QString refPlane);
    void setFps(qreal fps);
    void updateFps();
    void refreshImage();
    void refreshText();

private:
    ObjectTracker* _objectTracker;
    Aruco* _aruco;

    QImage _image;
    QImage _annotatedImage;
    qreal _fps;
    int _framesCounter;
    QElapsedTimer _elapsedTime;
    QTimer* _refreshImageTimer;
    int _refreshTextCounter;
    QTimer* _refreshFpsTimer;
    QMap<int, Track3dInfo*> _markerInfos;
    QString _refPlane;
};
