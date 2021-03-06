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
#include <QObject>
#include <QString>

class Marker;

class Track3dInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString x READ x NOTIFY changed)
    Q_PROPERTY(QString y READ y NOTIFY changed)
    Q_PROPERTY(QString z READ z NOTIFY changed)
    Q_PROPERTY(QString angle READ angle NOTIFY changed)
    Q_PROPERTY(QString fx READ fx NOTIFY changed)
    Q_PROPERTY(QString fy READ fy NOTIFY changed)
    Q_PROPERTY(QString fz READ fz NOTIFY changed)
    Q_PROPERTY(QString fangle READ fangle NOTIFY changed)

public:
    explicit Track3dInfo(int id, QObject* parent = nullptr);

    QString id() const;
    QString x() const;
    QString y() const;
    QString z() const;
    QString fx() const;
    QString fy() const;
    QString fz() const;
    QString angle() const;
    QString fangle() const;

    void update(Marker* marker);

signals:
    void changed();

private:
    QString _id, _x, _y, _z, _fx, _fy, _fz;
    QString _angle, _fangle;
};
