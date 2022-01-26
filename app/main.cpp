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
#include "Aruco/Aruco.h"
#include "Calibration/CalibrationController.h"
#include "Calibration/FramesCalibrationModel.h"
#include "Camera/CameraController.h"
#include "ImageItem.h"
#include "Record/RecordController.h"
#include "Replay/ReplayController.h"
#include "Track3d/Track3dInfo.h"
#include "Track3d/Track3dController.h"
#include "Video/Frame.h"
#include "Video/Marker.h"
#include "Video/Video.h"
#include "Video/VideoSource.h"
#include "Viewer/ViewerController.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setApplicationName(QStringLiteral("Aruco tracker"));
    QCoreApplication::setOrganizationName(QStringLiteral("Maken!"));
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterSingletonType(QUrl("qrc:/Style.qml"), "ArucoMarkerTracker", 1, 0, "Style");
    qmlRegisterType<VideoSource>("ArucoMarkerTracker", 1, 0, "VideoSource");
    qmlRegisterType<Aruco>("ArucoMarkerTracker", 1, 0, "Aruco");
    qmlRegisterType<CameraController>("ArucoMarkerTracker", 1, 0, "CameraController");
    qmlRegisterType<ReplayController>("ArucoMarkerTracker", 1, 0, "ReplayController");
    qmlRegisterType<RecordController>("ArucoMarkerTracker", 1, 0, "RecordController");
    qmlRegisterType<ViewerController>("ArucoMarkerTracker", 1, 0, "ViewerController");
    qmlRegisterType<CalibrationController>("ArucoMarkerTracker", 1, 0, "CalibrationController");
    qmlRegisterType<Track3dController>("ArucoMarkerTracker", 1, 0, "Track3dController");
    qmlRegisterUncreatableType<Track3dInfo>("ArucoMarkerTracker", 1, 0, "MarkerInfo", "Cannot create from qml");
    qmlRegisterUncreatableType<FramesCalibrationModel>("ArucoMarkerTracker", 1, 0, "FramesCalibrationModel", "Cannot create from qml");
    qmlRegisterType<ImageItem>("ArucoMarkerTracker", 1, 0, "ImageItem");

    engine.rootContext()->setContextProperty("globalVideoSource", new VideoSource(&app));
    engine.rootContext()->setContextProperty("globalAruco", new Aruco(&app));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    return app.exec();
}
