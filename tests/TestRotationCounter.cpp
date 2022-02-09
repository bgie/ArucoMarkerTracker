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
#include "TestRotationCounter.h"
#include "Kalman/RotationCounter.h"
#include "TestFactory.h"
#include <math.h>

REGISTER_TESTCLASS(TestRotationCounter);

void TestRotationCounter::angleWithRotations_should_handle_wrapping_angle()
{
    RotationCounter c;
    c.updateAngle(M_PI_2);
    QCOMPARE(c.angleWithRotations(), 1 * (float)M_PI_2);

    c.updateAngle(2 * M_PI_2);
    QCOMPARE(c.angleWithRotations(), 2 * (float)M_PI_2);

    c.updateAngle(-M_PI_2);
    QCOMPARE(c.angleWithRotations(), 3 * (float)M_PI_2);

    c.updateAngle(0);
    QCOMPARE(c.angleWithRotations(), 4 * (float)M_PI_2);

    c.updateAngle(M_PI_2);
    QCOMPARE(c.angleWithRotations(), 5 * (float)M_PI_2);

    c.updateAngle(2 * M_PI_2);
    QCOMPARE(c.angleWithRotations(), 6 * (float)M_PI_2);

    c.updateAngle(M_PI_2);
    QCOMPARE(c.angleWithRotations(), 5 * (float)M_PI_2);

    c.updateAngle(0);
    QCOMPARE(c.angleWithRotations(), 4 * (float)M_PI_2);

    c.updateAngle(-M_PI_2);
    QCOMPARE(c.angleWithRotations(), 3 * (float)M_PI_2);

    c.updateAngle(2 * M_PI_2);
    QCOMPARE(c.angleWithRotations(), 2 * (float)M_PI_2);

    c.updateAngle(M_PI_2);
    QCOMPARE(c.angleWithRotations(), 1 * (float)M_PI_2);

    c.updateAngle(0);
    QCOMPARE(c.angleWithRotations(), 0);

    c.updateAngle(-M_PI_2);
    QCOMPARE(c.angleWithRotations(), -1 * (float)M_PI_2);
}
