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

Marker::Marker(int id, QVector<QPointF> corners, QVector3D position, QObject* parent)
    : QObject(parent)
    , _id(id)
    , _corners(corners)
    , _position(position)
{
}

Marker::~Marker()
{
}

int Marker::id() const
{
    return _id;
}

QVector<QPointF> Marker::corners() const
{
    return _corners;
}

QVariantList Marker::cornersQVariants() const
{
    QVariantList result;
    for (auto corner : _corners) {
        result.append(QVariant(corner));
    }
    return result;
}

const QVector3D& Marker::position() const
{
    return _position;
}
