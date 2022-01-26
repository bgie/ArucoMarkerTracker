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

void Track3dInfo::setXYZ(float x, float y, float z)
{
    _x = QString::number(x, 'f', 1);
    _y = QString::number(y, 'f', 1);
    _z = QString::number(z, 'f', 1);
    emit changed();
}

void Track3dInfo::setNotDetected()
{
    _x = _y = _z = QStringLiteral("-");
    emit changed();
}
