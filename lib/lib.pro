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
    MainController.h \
    Marker.h \
    MarkerTracker.h \
    ReplayTimer.h \
    Video.h

SOURCES += \
    ArucoDetector.cpp \
    Camera.cpp \
    CameraCalibration.cpp \
    CameraReader.cpp \
    Frame.cpp \
    ImageSaver.cpp \
    MainController.cpp \
    Marker.cpp \
    MarkerTracker.cpp \
    ReplayTimer.cpp \
    Video.cpp
