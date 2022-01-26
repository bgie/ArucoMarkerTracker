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
#include "TestReferencePlaneTracker.h"
#include "ReferencePlaneTracker.h"
#include "TestFactory.h"
#include "Video/Marker.h"
#include <math.h>

REGISTER_TESTCLASS(TestReferencePlaneTracker);

namespace {
QSet<int> referenceIds({ 0, 1, 2, 3 });

QVector3D round(const QVector3D& input)
{
    return QVector3D(qRound(input.x()), qRound(input.y()), qRound(input.z()));
}
}

void TestReferencePlaneTracker::xy_plane_must_preserve_xy()
{
    //    ReferencePlaneTracker tut(referenceIds);

    //    QList<Marker*> markers;
    //    markers << new Marker(0, QVector3D(0, 0, 2), QVector3D());
    //    markers << new Marker(1, QVector3D(1, 0, 2), QVector3D());
    //    markers << new Marker(2, QVector3D(0, 1, 2), QVector3D());
    //    markers << new Marker(3, QVector3D(1, 1, 2), QVector3D());

    //    tut.trackMarkers(0, markers);
    //    QVERIFY(tut.hasPlane());

    //    QCOMPARE(round(tut.projectPoint(QPointF(3, 2))), QVector3D(3, 2, 2));

    //    for (Marker* m : markers) {
    //        QVector3D pos = m->position();
    //        QVector3D proj = round(tut.projectPoint(pos));
    //        QCOMPARE(proj, pos);
    //    }
}

void TestReferencePlaneTracker::test_angled_plane()
{
    //    ReferencePlaneTracker tut(referenceIds);

    //    QList<Marker*> markers;
    //    markers << new Marker(0, QVector3D(0, 0, 10), QVector3D());
    //    markers << new Marker(1, QVector3D(10, 0, 11), QVector3D());
    //    markers << new Marker(2, QVector3D(0, 10, 10), QVector3D());
    //    markers << new Marker(3, QVector3D(10, 10, 11), QVector3D());

    //    tut.trackMarkers(0, markers);
    //    QVERIFY(tut.hasPlane());

    //    for (Marker* m : markers) {
    //        QVector3D pos = m->position();
    //        QVector3D proj = round(tut.projectPoint(pos));
    //        QCOMPARE(proj, pos);
    //    }

    //    QCOMPARE(round(tut.projectPoint(QVector3D(3, 2, 42))), QVector3D(6, 2, 11));
}
