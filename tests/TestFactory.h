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
#ifndef TESTFACTORY_H
#define TESTFACTORY_H

#include <QList>
#include <QObject>
#include <QTest>
#include <functional>

#define REGISTER_TESTCLASS(c) const bool dummy = TestFactory::instance().registerTestConstructor([]() -> QObject* { return new c(); })

class TestFactory
{
private:
    TestFactory();

public:
    static TestFactory& instance();

    bool registerTestConstructor(std::function<QObject*()> f);
    int runAllTests(int argc, char* argv[]);

private:
    QList<std::function<QObject*()>> _constructors;
};

#endif // TESTFACTORY_H
