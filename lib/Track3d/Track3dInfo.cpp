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
#include "Track3dInfo.h"
#include <math.h>

Track3dInfo::Track3dInfo(int id, QObject* parent)
    : QObject(parent)
    , _id(QString::number(id))
    , _posFilter(KalmanTracker3D::movingTanksParams())
{
}

QString Track3dInfo::id() const
{
    return _id;
}

QString Track3dInfo::x() const
{
    return _x;
}

QString Track3dInfo::y() const
{
    return _y;
}

QString Track3dInfo::z() const
{
    return _z;
}

QString Track3dInfo::fx() const
{
    return _fx;
}

QString Track3dInfo::fy() const
{
    return _fy;
}

QString Track3dInfo::fz() const
{
    return _fz;
}

QString Track3dInfo::alfa() const
{
    return _alfa;
}

QString Track3dInfo::beta() const
{
    return _beta;
}

QString Track3dInfo::gamma() const
{
    return _gamma;
}

void Track3dInfo::setPositionRotation(float x, float y, float z, float alfa, float beta, float gamma)
{
    _posFilter.predict(1);
    _posFilter.update(QVector3D(x, y, z));

    _x = QString::number(x, 'f', 1);
    _y = QString::number(y, 'f', 1);
    _z = QString::number(z, 'f', 1);
    _alfa = QString::number(alfa * 180 / M_PI, 'f', 1);
    _beta = QString::number(beta * 180 / M_PI, 'f', 1);
    _gamma = QString::number(gamma * 180 / M_PI, 'f', 1);
    updateStrings();
    emit changed();
}

void Track3dInfo::setNotDetected()
{
    _posFilter.predict(1);

    _x = _y = _z = QStringLiteral("-");
    _alfa = _beta = _gamma = QStringLiteral("-");
    updateStrings();
    emit changed();
}

void Track3dInfo::updateStrings()
{
    if (_posFilter.hasPosition()) {
        _fx = QString::number(_posFilter.position().x(), 'f', 1);
        _fy = QString::number(_posFilter.position().y(), 'f', 1);
        _fz = QString::number(_posFilter.position().z(), 'f', 1);
    } else {
        _fx = _fy = _fz = QStringLiteral("-");
    }
}
