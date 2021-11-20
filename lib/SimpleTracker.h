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
#ifndef SIMPLETRACKER_H
#define SIMPLETRACKER_H

#include <QScopedPointer>
#include <QVector3D>

struct SimpleTrackerData;

class SimpleTracker {
public:
    class Params {
    public:
        Params();

        double positionProcessNoiseCov;
        double velocityProcessNoiseCov;
        double measurementNoiseCov;
    };

    SimpleTracker(const Params& p = Params());
    ~SimpleTracker();

    void update(double position);
    void updateNotFound();
    void predict(double elapsedMsec);

    bool hasPosition() const;
    double position() const;
    double velocity() const;

private:
    QScopedPointer<SimpleTrackerData> _d;
};

#endif // SIMPLETRACKER_H
