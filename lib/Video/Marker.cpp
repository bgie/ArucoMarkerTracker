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

namespace {
const QString ID(QStringLiteral("id"));
const QString POS_X(QStringLiteral("pos_x"));
const QString POS_Y(QStringLiteral("pos_y"));
const QString POS_Z(QStringLiteral("pos_z"));
const QString ROT_X(QStringLiteral("rot_x"));
const QString ROT_Y(QStringLiteral("rot_y"));
const QString ROT_Z(QStringLiteral("rot_z"));
}

Marker::Marker(int id, const QVector3D& position, const QVector3D& rotation, QPointF screenPos, QObject* parent)
    : QObject(parent)
    , _id(id)
    , _position(position)
    , _rotation(rotation)
    , _screenPos(screenPos)
{
}

Marker::Marker(QJsonObject obj, QObject* parent)
    : QObject(parent)
{
    _id = obj[ID].toInt();
    _position = QVector3D(obj[POS_X].toDouble(), obj[POS_Y].toDouble(), obj[POS_Z].toDouble());
    _rotation = QVector3D(obj[ROT_X].toDouble(), obj[ROT_Y].toDouble(), obj[ROT_Z].toDouble());
}

Marker::~Marker()
{
}

int Marker::id() const
{
    return _id;
}

const QVector3D& Marker::position() const
{
    return _position;
}

const QVector3D& Marker::rotation() const
{
    return _rotation;
}

const QPointF& Marker::screenPos() const
{
    return _screenPos;
}

QJsonObject Marker::toJson() const
{
    QJsonObject obj;
    obj.insert(ID, QJsonValue(_id));
    obj.insert(POS_X, QJsonValue(_position.x()));
    obj.insert(POS_Y, QJsonValue(_position.y()));
    obj.insert(POS_Z, QJsonValue(_position.z()));
    obj.insert(ROT_X, QJsonValue(_rotation.x()));
    obj.insert(ROT_Y, QJsonValue(_rotation.y()));
    obj.insert(ROT_Z, QJsonValue(_rotation.z()));
    return obj;
}
