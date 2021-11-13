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
#include "ArucoDetector.h"
#include "MarkerTracker.h"
#include <QString>
#include <QVector3D>
#include <QtConcurrent/QtConcurrentMap>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <qdebug.h>

using namespace cv;
using namespace cv::aruco;

const float ARUCO_MARKER_SIZE_MM = 32.0f;
const float ANNOTATE_AXIS_LENGTH = 25.0f;

struct ArucoDetectorData {
    Ptr<Dictionary> dictionary;
    Ptr<DetectorParameters> parameters;
    MarkerTracker tracker;
};

ArucoDetector::ArucoDetector()
    : _d(new ArucoDetectorData())
{
    _d->dictionary = getPredefinedDictionary(DICT_4X4_50);
    _d->parameters = DetectorParameters::create();
    _d->parameters->cornerRefinementMethod = CORNER_REFINE_SUBPIX;

    //    cv::Mat markerImage;
    //    for (int i = 0; i < 50; ++i) {
    //        drawMarker(_d->dictionary, i, 200, markerImage, 1);
    //        imwrite(QString("marker%1.png").arg(i).toLatin1().constData(), markerImage);
    //    }
}

ArucoDetector::~ArucoDetector()
{
}

void ArucoDetector::detectAruco(QImage& image, cv::Mat cameraMatrix, cv::Mat distCoeffs, bool kallmanFilter)
{
    if (image.size().isEmpty())
        return;

    std::vector<std::vector<Point2f>> markerCorners;
    std::vector<int> markerIds;
    cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
    detectMarkers(view, _d->dictionary, markerCorners, markerIds, _d->parameters, cv::noArray());

    drawDetectedMarkers(view, markerCorners, markerIds);

    if (!cameraMatrix.empty() && !distCoeffs.empty()) {
        std::vector<cv::Vec3d> rvecs, tvecs;
        estimatePoseSingleMarkers(markerCorners, ARUCO_MARKER_SIZE_MM, cameraMatrix, distCoeffs, rvecs, tvecs);

        int niceMarkerIndex = -1;
        for (size_t i = 0; i < rvecs.size(); ++i) {
            drawAxis(view, cameraMatrix, distCoeffs, rvecs.at(i), tvecs.at(i), ANNOTATE_AXIS_LENGTH);

            if (markerIds.at(i) == 1) {
                niceMarkerIndex = i;
            }

            //            cv::Vec3d filteredPos;
            //            std::vector<Point2f> imagePoints2;
            //            projectPoints(filteredPos, rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints2);
        }

        if (kallmanFilter) {
            _d->tracker.predict(1000.0f / 30.0f);
            if (niceMarkerIndex >= 0) {
                int i = niceMarkerIndex;
                QVector3D position(tvecs.at(i)[0], tvecs.at(i)[1], tvecs.at(i)[2]);
                _d->tracker.update(position);
                qDebug() << "Actual x:" << tvecs.at(i)[0] << " y:" << tvecs.at(i)[1];

            } else {
                _d->tracker.updateNotFound();
            }
        }
    }
}

void ArucoDetector::detectAllMarkers(QList<Frame*> frames, cv::Mat cameraMatrix, cv::Mat distCoeffs)
{
    if (frames.count() && !cameraMatrix.empty() && !distCoeffs.empty()) {
        QVector<int> range(frames.count()); // waiting for C++20 range class!
        for (int i = 0; i < range.size(); ++i)
            range[i] = i;

        QFuture<void> result = QtConcurrent::map(range, [=](int& index) -> void {
            Frame* frame = frames.at(index);

            std::vector<std::vector<Point2f>> markerCorners;
            std::vector<int> markerIds;
            cv::Mat view(frame->image().height(), frame->image().width(), CV_8UC3, (void*)frame->image().constBits(), frame->image().bytesPerLine());
            detectMarkers(view, _d->dictionary, markerCorners, markerIds, _d->parameters, cv::noArray(), cameraMatrix, distCoeffs);

            std::vector<cv::Vec3d> rvecs, tvecs;
            estimatePoseSingleMarkers(markerCorners, ARUCO_MARKER_SIZE_MM, cameraMatrix, distCoeffs, rvecs, tvecs);

            QList<Marker*> markers;
            for (size_t i = 0; i < markerIds.size(); ++i) {
                QVector<QPointF> corners;
                for (const Point2f& p : markerCorners.at(i)) {
                    corners.push_back(QPointF(p.x, p.y));
                }
                QVector3D position(tvecs.at(i)[0], tvecs.at(i)[1], tvecs.at(i)[2]);
                markers << new Marker(markerIds.at(i), corners, position);
            }
            qSort(markers.begin(), markers.end(), [](Marker* a, Marker* b) -> bool { return a->id() < b->id(); });
            frame->setMarkers(markers);
        });
        result.waitForFinished();
    }
}
