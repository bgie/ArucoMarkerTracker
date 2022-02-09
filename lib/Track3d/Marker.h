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
#include "Kalman/KalmanTracker1D.h"
#include "Kalman/KalmanTracker3D.h"
#include "Kalman/RotationCounter.h"
#include <QVector3D>

class Marker
{
public:
    explicit Marker(int id);

    void setPositionRotation(const QVector3D& newPos, float newAngle, float elapsedMsecs);
    void setNotDetected(float elapsedMsecs);

    bool isDetected() const;
    QVector3D pos() const;
    float angle() const;

    bool isDetectedFiltered() const;
    QVector3D filteredPos() const;
    float filteredAngle() const;

private:
    const int _id;
    bool _isDetected;
    QVector3D _pos;
    float _angle;
    KalmanTracker3D _posFilter;
    RotationCounter _rotationCounter;
    KalmanTracker1D _angleFilter;
};
