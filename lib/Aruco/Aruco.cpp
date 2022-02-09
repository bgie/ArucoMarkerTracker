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
#include "Aruco.h"
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

struct Aruco::Data {
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> parameters;
    float markerLengthInMm;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
};

Aruco::Aruco(QObject* parent)
    : QObject(parent)
    , _d(new Data())
{
    _d->dictionary = getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    _d->parameters = cv::aruco::DetectorParameters::create();
    _d->parameters->cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
    _d->markerLengthInMm = 32.0f;
}

Aruco::~Aruco()
{
}

void Aruco::setCameraMatrix(cv::Mat cameraMatrix, cv::Mat distCoeffs)
{
    _d->cameraMatrix = cameraMatrix;
    _d->distCoeffs = distCoeffs;
}

Aruco::Markers Aruco::detectMarkers(QImage image) const
{
    Markers result;
    if (!image.size().isEmpty() && !_d->cameraMatrix.empty() && !_d->distCoeffs.empty()) {
        cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::aruco::detectMarkers(view, _d->dictionary, result.corners, result.ids, _d->parameters, cv::noArray(), _d->cameraMatrix, _d->distCoeffs);
        cv::aruco::estimatePoseSingleMarkers(result.corners, _d->markerLengthInMm, _d->cameraMatrix, _d->distCoeffs, result.rvecs, result.tvecs);
    }
    return result;
}

std::vector<float> Aruco::calc2dAngles(const Aruco::Markers& markers) const
{
    std::vector<cv::Point3f> axesPoints;
    axesPoints.push_back(cv::Point3f(0, 0, 0));
    axesPoints.push_back(cv::Point3f(1, 0, 0));
    std::vector<float> results;

    for (uint i = 0; i < markers.rvecs.size(); ++i) {
        std::vector<cv::Point2f> proj;
        projectPoints(axesPoints, markers.rvecs.at(i), markers.tvecs.at(i), _d->cameraMatrix, _d->distCoeffs, proj);
        results.push_back(atan2(proj.at(1).y - proj.at(0).y, proj.at(1).x - proj.at(0).x));
    }
    return results;
}

void Aruco::drawMarkers(QImage& image, const Aruco::Markers& markers) const
{
    if (!image.size().isEmpty()) {
        cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.bits(), image.bytesPerLine());
        cv::aruco::drawDetectedMarkers(view, markers.corners, markers.ids);
    }
}

void Aruco::generateMarkerImageFiles(QString path) const
{
    // TODO

    //    cv::Mat markerImage;
    //    for (int i = 0; i < 50; ++i) {
    //        drawMarker(_d->dictionary, i, 200, markerImage, 1);
    //        imwrite(QString("marker%1.png").arg(i).toLatin1().constData(), markerImage);
    //    }
}
