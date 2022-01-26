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
#include <QList>
#include <QVector3D>

// A 3d plane with equation ax + by + cz + d = 0
class Plane3d
{
public:
    Plane3d(double a = 0.0, double b = 0.0, double c = 0.0, double d = 0.0);

    static Plane3d fitToPoints(QList<QVector3D> points, bool* ok);

    double a() const;
    double b() const;
    double c() const;
    double d() const;

    double xAngle() const;
    double yAngle() const;
    double zAngle() const;

private:
    double _a, _b, _c, _d;
    double _l2abc;
};
