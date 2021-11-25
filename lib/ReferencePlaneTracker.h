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
#ifndef REFERENCEPLANETRACKER_H
#define REFERENCEPLANETRACKER_H

#include <QList>
#include <QScopedPointer>
#include <QSet>

class Marker;

class ReferencePlaneTracker
{
public:
    ReferencePlaneTracker(QSet<int> referenceMarkerIds);
    ~ReferencePlaneTracker();

    bool hasPlane() const;
    void trackMarkers(double elapsedMsecs, QList<Marker*> markers);

    QVector3D centerPoint() const;
    QList<QVector3D> cornerPoints() const;
    QVector3D projectPoint(const QPointF& input) const;

private:
    struct Data;
    QScopedPointer<Data> _d;
};

#endif // REFERENCEPLANETRACKER_H
