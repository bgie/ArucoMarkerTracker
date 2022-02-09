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
#include "Marker.h"
#include <math.h>

Track3dInfo::Track3dInfo(int id, QObject* parent)
    : QObject(parent)
    , _id(QString::number(id))
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

QString Track3dInfo::angle() const
{
    return _angle;
}

QString Track3dInfo::fangle() const
{
    return _fangle;
}

void Track3dInfo::update(Marker* marker)
{
    if (marker->isDetected()) {
        _x = QString::number(marker->pos().x(), 'f', 1);
        _y = QString::number(marker->pos().y(), 'f', 1);
        _z = QString::number(marker->pos().z(), 'f', 1);
        _angle = QString::number(marker->angle() * 180 / M_PI, 'f', 1);
    } else {
        _x = _y = _z = _angle = QStringLiteral("-");
    }
    if (marker->isDetectedFiltered()) {
        _fx = QString::number(marker->filteredPos().x(), 'f', 1);
        _fy = QString::number(marker->filteredPos().y(), 'f', 1);
        _fz = QString::number(marker->filteredPos().z(), 'f', 1);
        _fangle = QString::number(marker->filteredAngle() * 180 / M_PI, 'f', 1);
    } else {
        _fx = _fy = _fz = _fangle = QStringLiteral("-");
    }
    emit changed();
}
