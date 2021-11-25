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
    ArucoDetector.h \
    Camera.h \
    CameraCalibration.h \
    CameraReader.h \
    Frame.h \
    ImageSaver.h \
    KalmanTracker1D.h \
    KalmanTracker3D.h \
    MainController.h \
    Marker.h \
    ReferencePlaneTracker.h \
    ReplayTimer.h \
    TrackAllMarkers.h \
    Video.h \
    GeneticAlgorithm.h

SOURCES += \
    ArucoDetector.cpp \
    Camera.cpp \
    CameraCalibration.cpp \
    CameraReader.cpp \
    Frame.cpp \
    ImageSaver.cpp \
    KalmanTracker1D.cpp \
    KalmanTracker3D.cpp \
    MainController.cpp \
    Marker.cpp \
    ReferencePlaneTracker.cpp \
    ReplayTimer.cpp \
    TrackAllMarkers.cpp \
    Video.cpp \
    GeneticAlgorithm.cpp
