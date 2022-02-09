#    ArucoMarkerTracker
#    Copyright (C) 2021 Kuppens Brecht
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
TEMPLATE = lib
CONFIG += staticlib c++17
QT += quick widgets concurrent

include(../link_opencv.pri)

HEADERS += \
    Aruco/Aruco.h \
    Calibration/CalibrationController.h \
    Calibration/FramesCalibrationModel.h \
    Camera/Camera.h \
    Camera/CameraController.h \
    Calibration/CameraCalibration.h \
    Camera/CameraReader.h \
    Kalman/KalmanTracker1D.h \
    Kalman/KalmanTracker3D.h \
    Kalman/RotationCounter.h \
    Track3d/Marker.h \
    Track3d/ObjectTracker.h \
    Track3d/Plane3d.h \
    Track3d/Track3dController.h \
    Track3d/Track3dInfo.h \
    Video/Frame.h \
    Record/ImageSaver.h \
    Replay/ReplayController.h \
    Replay/ReplayTimer.h \
    Record/RecordController.h \
    Video/Video.h \
    Video/VideoSource.h \
    Viewer/ViewerController.h

SOURCES += \
    Aruco/Aruco.cpp \
    Calibration/CalibrationController.cpp \
    Calibration/FramesCalibrationModel.cpp \
    Camera/Camera.cpp \
    Camera/CameraController.cpp \
    Calibration/CameraCalibration.cpp \
    Camera/CameraReader.cpp \
    Kalman/KalmanTracker1D.cpp \
    Kalman/KalmanTracker3D.cpp \
    Kalman/RotationCounter.cpp \
    Track3d/Marker.cpp \
    Track3d/ObjectTracker.cpp \
    Track3d/Plane3d.cpp \
    Track3d/Track3dController.cpp \
    Track3d/Track3dInfo.cpp \
    Video/Frame.cpp \
    Record/ImageSaver.cpp \
    Replay/ReplayController.cpp \
    Replay/ReplayTimer.cpp \
    Record/RecordController.cpp \
    Video/Video.cpp \
    Video/VideoSource.cpp \
    Viewer/ViewerController.cpp

