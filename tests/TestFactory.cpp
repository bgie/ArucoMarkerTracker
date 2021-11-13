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
#include "TestFactory.h"
#include <QtTest/QTest>
#include <iostream>

TestFactory::TestFactory()
{
}

TestFactory& TestFactory::instance()
{
    static TestFactory fac;
    return fac;
}

bool TestFactory::registerTestConstructor(std::function<QObject*()> f)
{
    _constructors << f;
    return true;
}

int TestFactory::runAllTests(int argc, char* argv[])
{
    int result = 0;

    for (int i = 0; i < _constructors.size(); ++i) {
        std::cout << "********* Test " << i + 1 << " of " << _constructors.size() << " *********" << std::endl;

        QScopedPointer<QObject> testObject(_constructors.at(i)());
        int testResult = QTest::qExec(testObject.data(), argc, argv);
        result |= testResult;
    }
    std::cout << "********* All tests: " << (result == 0 ? "PASS" : "FAIL") << " *********" << std::endl;
    return result;
}
