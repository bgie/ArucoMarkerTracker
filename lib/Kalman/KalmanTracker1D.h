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
#include <QScopedPointer>
#include <QVector3D>

class KalmanTracker1D {
public:
    struct Params {
        Params(double positionProcessNoiseCov = 1, double velocityProcessNoiseCov = 1, double measurementNoiseCov = 1, double notUpdatedTimeoutInMsec = 3000);

        double positionProcessNoiseCov;
        double velocityProcessNoiseCov;
        double measurementNoiseCov;
        double notUpdatedTimeoutInMsec;
    };

    KalmanTracker1D(const Params& p = Params());
    ~KalmanTracker1D();

    void update(double position);
    void predict(double elapsedMsec);

    bool hasPosition() const;
    double position() const;
    double velocity() const;
    
    static const Params& movingTanksParams();
    static const Params& staticMarkerParams();
    
private:
    struct Data;
    QScopedPointer<Data> _d;
};
