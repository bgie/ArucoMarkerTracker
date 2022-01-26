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
    ArucoDetector.h \
    Calibration/CalibrationController.h \
    Calibration/FramesCalibrationModel.h \
    Camera/Camera.h \
    Camera/CameraController.h \
    Calibration/CameraCalibration.h \
    Camera/CameraReader.h \
    Track3d/MarkerInfo.h \
    Track3d/Track3dController.h \
    Video/Frame.h \
    Record/ImageSaver.h \
    KalmanTracker1D.h \
    KalmanTracker3D.h \
    Video/Marker.h \
    ReferencePlaneTracker.h \
    Replay/ReplayController.h \
    Replay/ReplayTimer.h \
    Record/RecordController.h \
    TrackAllMarkers.h \
    Video/Video.h \
    GeneticAlgorithm.h \
    Video/VideoSource.h \
    Viewer/ViewerController.h

SOURCES += \
    Aruco/Aruco.cpp \
    ArucoDetector.cpp \
    Calibration/CalibrationController.cpp \
    Calibration/FramesCalibrationModel.cpp \
    Camera/Camera.cpp \
    Camera/CameraController.cpp \
    Calibration/CameraCalibration.cpp \
    Camera/CameraReader.cpp \
    Track3d/MarkerInfo.cpp \
    Track3d/Track3dController.cpp \
    Video/Frame.cpp \
    Record/ImageSaver.cpp \
    KalmanTracker1D.cpp \
    KalmanTracker3D.cpp \
    Video/Marker.cpp \
    ReferencePlaneTracker.cpp \
    Replay/ReplayController.cpp \
    Replay/ReplayTimer.cpp \
    Record/RecordController.cpp \
    TrackAllMarkers.cpp \
    Video/Video.cpp \
    GeneticAlgorithm.cpp \
    Video/VideoSource.cpp \
    Viewer/ViewerController.cpp

