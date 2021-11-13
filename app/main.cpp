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
#include "Frame.h"
#include "ImageItem.h"
#include "MainController.h"
#include "Video.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setApplicationName(QStringLiteral("Aruco tracker"));
    QCoreApplication::setOrganizationName(QStringLiteral("Bgie"));
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterSingletonType( QUrl("qrc:/Style.qml"), "MyApp", 1, 0, "Style" );
    qmlRegisterType<MainController>("MyApp", 1, 0, "MainController");
    qmlRegisterType<ImageItem>("MyApp", 1, 0, "ImageItem");
    qmlRegisterType<Video>("MyApp", 1, 0, "Video");
    qmlRegisterUncreatableType<Frame>("MyApp", 1, 0, "Frame", QStringLiteral("Frame not createable from qml"));
    qmlRegisterUncreatableType<Marker>("MyApp", 1, 0, "Marker", QStringLiteral("Marker not createable from qml"));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    return app.exec();
}
