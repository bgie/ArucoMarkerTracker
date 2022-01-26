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
#include "MarkerInfo.h"

MarkerInfo::MarkerInfo(int id, QObject* parent)
    : QObject(parent)
    , _id(QString::number(id))
{
}

QString MarkerInfo::id() const
{
    return _id;
}

QString MarkerInfo::x() const
{
    return _x;
}

QString MarkerInfo::y() const
{
    return _y;
}

QString MarkerInfo::z() const
{
    return _z;
}

void MarkerInfo::setXYZ(float x, float y, float z)
{
    _x = QString::number(x, 'f', 1);
    _y = QString::number(y, 'f', 1);
    _z = QString::number(z, 'f', 1);
    emit changed();
}

void MarkerInfo::setNotDetected()
{
    _x = _y = _z = QStringLiteral("-");
    emit changed();
}
