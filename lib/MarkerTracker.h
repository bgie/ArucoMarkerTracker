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
#ifndef MARKERTRACKER_H
#define MARKERTRACKER_H

#include <QScopedPointer>
#include <QVector3D>

struct MarkerTrackerData;

class MarkerTracker
{
public:
    class Params {
    public:
        Params();

        double positionXYProcessNoiseCov;
        double positionZProcessNoiseCov;
        double velocityXYProcessNoiseCov;
        double velocityZProcessNoiseCov;
        double measurementXYNoiseCov;
        double measurementZNoiseCov;
        double positionXYerrorCovPost;
        double positionZerrorCovPost;
        double velocityXYerrorCovPost;
        double velocityZerrorCovPost;
        double posVelXYProcessNoiseCov;
        double posVelZProcessNoiseCov;
    };

    MarkerTracker(const Params& p = Params());
    ~MarkerTracker();

    void update(const QVector3D& position, const QVector3D& rotation);
    void updateNotFound();
    void predict(double elapsedMsec);

    bool hasPosition() const;
    QVector3D position() const;
    QVector3D rotation() const;

private:
    QScopedPointer<MarkerTrackerData> _d;
};

#endif // MARKERTRACKER_H
