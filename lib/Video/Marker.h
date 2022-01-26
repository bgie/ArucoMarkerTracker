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
#include <QJsonObject>
#include <QObject>
#include <QVector3D>

class Marker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QVector3D position READ position CONSTANT)

public:
    explicit Marker(int id, const QVector3D& position, const QVector3D& rotation, QPointF screenPos = QPointF(), QObject* parent = nullptr);
    explicit Marker(QJsonObject object, QObject* parent = nullptr);
    virtual ~Marker() override;

    int id() const;
    const QVector3D& position() const;
    const QVector3D& rotation() const;
    const QPointF& screenPos() const;

    QJsonObject toJson() const;

private:
    int _id;
    QVector3D _position;
    QVector3D _rotation;
    QPointF _screenPos;
};
