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
#include "Track3d/ObjectTracker.h"
#include "Track3d/Track3dController.h"
#include "Track3d/Track3dInfo.h"
#include "Video/Frame.h"
#include "Video/Video.h"
#include "Viewer/ViewerController.h"
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>

Q_DECLARE_METATYPE(QElapsedTimer)
int metatype_id = qRegisterMetaType<QElapsedTimer>("QElapsedTimer");

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setApplicationName(QStringLiteral("Aruco tracker"));
    QCoreApplication::setOrganizationName(QStringLiteral("Maken!"));
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType(QUrl("qrc:/Style.qml"), "ArucoMarkerTracker", 1, 0, "Style");
    qmlRegisterType<Aruco>("ArucoMarkerTracker", 1, 0, "Aruco");
    qmlRegisterUncreatableType<ObjectTracker>("ArucoMarkerTracker", 1, 0, "ObjectTracker", "Cannot create from qml");
    qmlRegisterType<CameraController>("ArucoMarkerTracker", 1, 0, "CameraController");
    qmlRegisterType<ReplayController>("ArucoMarkerTracker", 1, 0, "ReplayController");
    qmlRegisterType<RecordController>("ArucoMarkerTracker", 1, 0, "RecordController");
    qmlRegisterType<ViewerController>("ArucoMarkerTracker", 1, 0, "ViewerController");
    qmlRegisterType<CalibrationController>("ArucoMarkerTracker", 1, 0, "CalibrationController");
    qmlRegisterType<Track3dController>("ArucoMarkerTracker", 1, 0, "Track3dController");
    qmlRegisterUncreatableType<Track3dInfo>("ArucoMarkerTracker", 1, 0, "MarkerInfo", "Cannot create from qml");
    qmlRegisterUncreatableType<FramesCalibrationModel>("ArucoMarkerTracker", 1, 0, "FramesCalibrationModel", "Cannot create from qml");
    qmlRegisterType<ImageItem>("ArucoMarkerTracker", 1, 0, "ImageItem");

    Aruco aruco;
    QThread* trackingThread = new QThread(&app);
    ObjectTracker tracker(&aruco);
    tracker.moveToThread(trackingThread);
    trackingThread->start(QThread::TimeCriticalPriority);
    CameraController cameraController;
    QObject::connect(&cameraController, &CameraController::imageChanged, &tracker, &ObjectTracker::processFrame, Qt::QueuedConnection);
    RecordController recordController;
    QObject::connect(&cameraController, &CameraController::imageChanged, &recordController, &RecordController::setImage, Qt::QueuedConnection);
    ReplayController replayController;
    QObject::connect(&replayController, &ReplayController::imageChanged, &tracker, &ObjectTracker::processFrame, Qt::QueuedConnection);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("globalAruco", &aruco);
    engine.rootContext()->setContextProperty("globalObjectTracker", &tracker);
    engine.rootContext()->setContextProperty("globalCameraController", &cameraController);
    engine.rootContext()->setContextProperty("globalRecordController", &recordController);
    engine.rootContext()->setContextProperty("globalReplayController", &replayController);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    auto result = app.exec();
    trackingThread->quit();
    trackingThread->wait();
    return result;
}
