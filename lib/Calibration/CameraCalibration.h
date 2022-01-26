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
#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

#include "Video/Frame.h"
#include <QImage>
#include <QScopedPointer>
#include <opencv2/core/mat.hpp>

struct CameraCalibrationData;

class CameraCalibration
{
public:
    CameraCalibration();
    ~CameraCalibration();

    void annotateCheckerBoard(QImage& image);

    double calibrateUsingVideo(QList<Frame*> frames);
    void save(QString filename);
    void load(QString filename);

    void undistortImage(QImage& image);

    cv::Mat cameraMatrix() const;
    cv::Mat distCoeffs() const;

    QString calibrationValues() const;

private:
    QScopedPointer<CameraCalibrationData> _d;
};

#endif // CAMERACALIBRATION_H
