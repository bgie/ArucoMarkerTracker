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
#include "Marker.h"

Marker::Marker(int id)
    : _id(id)
    , _posFilter(KalmanTracker3D::movingTanksParams())
    , _angleFilter(KalmanTracker1D::movingTanksParams())
{
}

void Marker::setPositionRotation(const QVector3D& newPos, float newAngle, float elapsedMsecs)
{
    _pos = newPos;
    _posFilter.predict(elapsedMsecs);
    _posFilter.update(newPos);

    _angleFilter.predict(elapsedMsecs);
    _rotationCounter.updateAngle(newAngle);
    _angle = _rotationCounter.angleWithRotations();
    _angleFilter.update(_rotationCounter.angleWithRotations());

    _isDetected = true;
}

void Marker::setNotDetected(float elapsedMsecs)
{
    _posFilter.predict(elapsedMsecs);
    _angleFilter.predict(elapsedMsecs);

    _isDetected = false;
}

bool Marker::isDetected() const
{
    return _isDetected;
}

QVector3D Marker::pos() const
{
    return _pos;
}

float Marker::angle() const
{
    return _angle;
}

bool Marker::isDetectedFiltered() const
{
    return _posFilter.hasPosition() && _angleFilter.hasPosition();
}

QVector3D Marker::filteredPos() const
{
    return _posFilter.position();
}

float Marker::filteredAngle() const
{
    return _angleFilter.position();
}
