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
#include "Plane3d.h"
#include <math.h>
#include <opencv2/core.hpp>

Plane3d::Plane3d(double a, double b, double c, double d)
    : _a(a)
    , _b(b)
    , _c(c)
    , _d(d)
    , _l2abc(sqrt(a * a + b * b + c * c))
{
}

Plane3d Plane3d::fitToPoints(QList<QVector3D> points, bool* ok)
{
    Plane3d result;
    bool hasPlane = false;

    if (points.size() >= 3) {
        cv::Mat_<double> pointsXYZ1(points.size() + 1, 4, 1.0);
        cv::Mat_<double> zeros(points.size() + 1, 1, 0.0);
        cv::Mat_<double> planeABCD;
        for (int i = 0; i < points.size(); ++i) {
            pointsXYZ1.at<double>(i, 0) = points.at(i).x();
            pointsXYZ1.at<double>(i, 1) = points.at(i).y();
            pointsXYZ1.at<double>(i, 2) = points.at(i).z();
        }
        pointsXYZ1.at<double>(points.size(), 0) = 1.0;
        pointsXYZ1.at<double>(points.size(), 1) = 1.0;
        pointsXYZ1.at<double>(points.size(), 2) = 1.0;
        zeros.at<double>(points.size(), 0) = 1.0;

        hasPlane = cv::solve(pointsXYZ1, zeros, planeABCD, cv::DECOMP_QR);

        if (hasPlane) {
            double a = planeABCD.at<double>(0, 0);
            double b = planeABCD.at<double>(1, 0);
            double c = planeABCD.at<double>(2, 0);
            double d = planeABCD.at<double>(3, 0);
            result = Plane3d(a, b, c, d);
        }
    }

    if (ok) {
        *ok = hasPlane;
    }
    return result;
}

double Plane3d::a() const
{
    return _a;
}

double Plane3d::b() const
{
    return _b;
}

double Plane3d::c() const
{
    return _c;
}

double Plane3d::d() const
{
    return _d;
}

double Plane3d::xAngle() const
{
    return acos(_a / _l2abc);
}

double Plane3d::yAngle() const
{
    return acos(_b / _l2abc);
}

double Plane3d::zAngle() const
{
    return acos(_c / _l2abc);
}
