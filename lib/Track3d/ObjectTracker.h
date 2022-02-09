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
#include "Aruco/Aruco.h"
#include <QMap>
#include <QMutex>
#include <QObject>

class Marker;

class ObjectTracker : public QObject {
    Q_OBJECT
    Q_PROPERTY(float framesPerSecond READ framesPerSecond WRITE setFramesPerSecond NOTIFY framesPerSecondChanged)

public:
    explicit ObjectTracker(Aruco* aruco, QObject* parent = nullptr);
    virtual ~ObjectTracker() override;

    void processFrame(QImage image);

    QMutex* mutex();

    // *** methods below must be called with locked mutex -->

    float framesPerSecond() const;
    void setFramesPerSecond(float framesPerSecond);

    QImage image() const;
    const Aruco::Markers& markers() const;
    QMap<int, Marker*> idToMarker() const;

    // <-- end mutex lock ***

signals:
    void framesPerSecondChanged(float framesPerSecond);
    void imageChanged(QImage image);

private:
    mutable QMutex _mutex;
    QImage _image;
    Aruco* const _aruco;
    Aruco::Markers _markers;
    QMap<int, Marker*> _idToMarker;
    float _framesPerSecond;
};
