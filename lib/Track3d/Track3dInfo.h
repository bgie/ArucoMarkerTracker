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
#include <QObject>
#include <QString>

class Track3dInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString x READ x NOTIFY changed)
    Q_PROPERTY(QString y READ y NOTIFY changed)
    Q_PROPERTY(QString z READ z NOTIFY changed)
    Q_PROPERTY(QString fx READ fx NOTIFY changed)
    Q_PROPERTY(QString fy READ fy NOTIFY changed)
    Q_PROPERTY(QString fz READ fz NOTIFY changed)
    Q_PROPERTY(QString alfa READ alfa NOTIFY changed)
    Q_PROPERTY(QString beta READ beta NOTIFY changed)
    Q_PROPERTY(QString gamma READ gamma NOTIFY changed)
    Q_PROPERTY(QString angle2d READ angle2d NOTIFY changed)
    Q_PROPERTY(QString fangle2d READ fangle2d NOTIFY changed)

public:
    explicit Track3dInfo(int id, QObject* parent = nullptr);

    QString id() const;
    QString x() const;
    QString y() const;
    QString z() const;
    QString fx() const;
    QString fy() const;
    QString fz() const;
    QString alfa() const;
    QString beta() const;
    QString gamma() const;
    QString angle2d() const;
    QString fangle2d() const;

    void setPositionRotation(float x, float y, float z, float alfa, float beta, float gamma, float angle2d);
    void setNotDetected();

signals:
    void changed();

private:
    void updateStrings();

private:
    QString _id, _x, _y, _z, _fx, _fy, _fz;
    QString _alfa, _beta, _gamma, _angle2d, _fangle2;
    KalmanTracker3D _posFilter;
    RotationCounter _rotationCounter;
    KalmanTracker1D _angleFilter;
};
