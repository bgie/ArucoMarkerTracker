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
#ifndef ARUCODETECTOR_H
#define ARUCODETECTOR_H

#include "Frame.h"
#include <QImage>
#include <QScopedPointer>
#include <opencv2/core.hpp>

struct ArucoDetectorData;

class ArucoDetector
{
public:
    ArucoDetector();
    ~ArucoDetector();

    void detectAruco(QImage& image, cv::Mat cameraMatrix, cv::Mat distCoeffs, bool kallmanFilter);

    void detectAllMarkers(QList<Frame*> frames, cv::Mat cameraMatrix, cv::Mat distCoeffs);

private:
    QScopedPointer<ArucoDetectorData> _d;
};

#endif // ARUCODETECTOR_H
