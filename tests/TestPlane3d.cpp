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
#include "TestPlane3d.h"
#include "TestFactory.h"
#include "Track3d/Plane3d.h"

REGISTER_TESTCLASS(TestPlane3d);

void TestPlane3d::constructor_should_initialize_properties()
{
    Plane3d plane(1, 2, 3, 4);

    QCOMPARE(plane.a(), 1);
    QCOMPARE(plane.b(), 2);
    QCOMPARE(plane.c(), 3);
    QCOMPARE(plane.d(), 4);
}

void TestPlane3d::fit_plane_should_give_expected_results()
{
    QFETCH(QList<QVector3D>, points);
    QFETCH(bool, ok);
    QFETCH(double, a);
    QFETCH(double, b);
    QFETCH(double, c);
    QFETCH(double, d);

    bool resultOk = false;
    Plane3d plane = Plane3d::fitToPoints(points, &resultOk);

    QCOMPARE(resultOk, ok);

    if (qFuzzyIsNull(a))
        QVERIFY(qFuzzyIsNull(plane.a()));
    else
        QCOMPARE(plane.a(), a);

    if (qFuzzyIsNull(b))
        QVERIFY(qFuzzyIsNull(plane.b()));
    else
        QCOMPARE(plane.b(), b);

    if (qFuzzyIsNull(c))
        QVERIFY(qFuzzyIsNull(plane.c()));
    else
        QCOMPARE(plane.c(), c);

    if (qFuzzyIsNull(d))
        QVERIFY(qFuzzyIsNull(plane.d()));
    else
        QCOMPARE(plane.d(), d);
}

void TestPlane3d::fit_plane_should_give_expected_results_data()
{
    QTest::addColumn<QList<QVector3D>>("points");
    QTest::addColumn<bool>("ok");
    QTest::addColumn<double>("a");
    QTest::addColumn<double>("b");
    QTest::addColumn<double>("c");
    QTest::addColumn<double>("d");

    QTest::newRow("no points")
        << (QList<QVector3D>())
        << false
        << 0.0 << 0.0 << 0.0 << 0.0;

    QTest::newRow("underdetermined")
        << (QList<QVector3D>() << QVector3D(0, 0, 0) << QVector3D(0, 1, 0))
        << false
        << 0.0 << 0.0 << 0.0 << 0.0;

    QTest::newRow("x plane")
        << (QList<QVector3D>() << QVector3D(0, 0, 0) << QVector3D(0, 1, 0) << QVector3D(0, 0, 1))
        << true
        << 1.0 << 0.0 << 0.0 << 0.0;

    QTest::newRow("y plane")
        << (QList<QVector3D>() << QVector3D(0, 0, 0) << QVector3D(1, 0, 0) << QVector3D(0, 0, 1))
        << true
        << 0.0 << 1.0 << 0.0 << 0.0;

    QTest::newRow("z plane")
        << (QList<QVector3D>() << QVector3D(0, 0, 0) << QVector3D(1, 0, 0) << QVector3D(0, 1, 0))
        << true
        << 0.0 << 0.0 << 1.0 << 0.0;

    QTest::newRow("example from online calculator")
        << (QList<QVector3D>() << QVector3D(1, 2, -2) << QVector3D(3, -2, 1) << QVector3D(5, 1, -4))
        << true
        << 0.423076923077 << 0.615384615385 << 0.538461538462 << -0.576923076923;
}
