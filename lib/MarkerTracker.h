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
    MarkerTracker();
    ~MarkerTracker();

    void update(const QVector3D& position);
    void updateNotFound();
    void predict(float elapsedMsec);

    bool hasPosition() const;
    QVector3D position() const;

private:
    QScopedPointer<MarkerTrackerData> _d;
};

#endif // MARKERTRACKER_H
