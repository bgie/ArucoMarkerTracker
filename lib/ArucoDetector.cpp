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
#include "KalmanTracker3D.h"
#include "ReferencePlaneTracker.h"
#include "Video/Marker.h"
#include <QHash>
#include <QString>
#include <QVector3D>
#include <QtConcurrent/QtConcurrentMap>
#include <math.h>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <qdebug.h>

using namespace cv;
using namespace cv::aruco;

namespace {

const float ARUCO_MARKER_SIZE_MM = 32.0f;
const float ANNOTATE_AXIS_LENGTH = 150.0f;
const float ANNOTATE_CENTER_POINT_SIZE = 10.0f;

void annotate(InputOutputArray img, bool filtered, InputArray rvec, InputArray tvec, InputArray cameraMatrix, InputArray distCoeffs)
{
    const Scalar& color = filtered ? Scalar(0, 255, 255) : Scalar(0, 0, 255);
    const int width = filtered ? 2 : 5;
    std::vector<Point3f> axesPoints;
    axesPoints.push_back(Point3f(0, 0, 0));
    axesPoints.push_back(Point3f(ANNOTATE_AXIS_LENGTH, 0, 0));
    //if (!filtered) {
    axesPoints.push_back(Point3f(ANNOTATE_CENTER_POINT_SIZE, 0, 0));
    //}
    std::vector<Point2f> imagePoints;
    projectPoints(axesPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

    cv::line(img, imagePoints[0], imagePoints[1], color, width);

    //if (!filtered) {
    Point2f centerSize = (imagePoints[2] - imagePoints[0]);
    const int FIXEDPOINT_BITS = 8;
    const int FIXEDPOINT_MULTIPLIER = (1 << FIXEDPOINT_BITS);
    int radius = static_cast<int>(round(FIXEDPOINT_MULTIPLIER * sqrt(centerSize.x * centerSize.x + centerSize.y * centerSize.y)));
    Point2i center(round(imagePoints[0].x * FIXEDPOINT_MULTIPLIER), round(imagePoints[0].y * FIXEDPOINT_MULTIPLIER));
    cv::circle(img, center, radius, color, cv::FILLED, cv::FILLED, FIXEDPOINT_BITS);
    //}
}

}

struct ArucoDetectorData {
    ArucoDetectorData()
        : refPlane(QSet<int>({ 0, 1, 2, 3 }))
    {
    }

    Ptr<Dictionary> dictionary;
    Ptr<DetectorParameters> parameters;
    QHash<int, KalmanTracker3D*> trackers;
    ReferencePlaneTracker refPlane;
};

ArucoDetector::ArucoDetector()
    : _d(new ArucoDetectorData())
{
    _d->dictionary = getPredefinedDictionary(DICT_4X4_50);
    _d->parameters = DetectorParameters::create();
    _d->parameters->cornerRefinementMethod = CORNER_REFINE_NONE; // CORNER_REFINE_SUBPIX;
}

ArucoDetector::~ArucoDetector()
{
    qDeleteAll(_d->trackers.values());
}

void ArucoDetector::detectAruco(QImage& image, cv::Mat cameraMatrix, cv::Mat distCoeffs, bool kallmanFilter)
{
    if (image.size().isEmpty())
        return;

    if (!cameraMatrix.empty() && !distCoeffs.empty()) {
        std::vector<std::vector<Point2f>> markerCorners;
        std::vector<int> markerIds;
        cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        detectMarkers(view, _d->dictionary, markerCorners, markerIds, _d->parameters, cv::noArray(), cameraMatrix, distCoeffs);

        std::vector<cv::Vec3d> rvecs, tvecs;
        estimatePoseSingleMarkers(markerCorners, ARUCO_MARKER_SIZE_MM, cameraMatrix, distCoeffs, rvecs, tvecs);

        QSet<int> foundIds;
        QList<Marker*> markers;

        for (size_t i = 0; i < rvecs.size(); ++i) {
            QPointF screenPos;
            for (const Point2f& p : markerCorners.at(i)) {
                screenPos.rx() += p.x;
                screenPos.ry() += p.y;
            }
            screenPos /= markerCorners.at(i).size();
            cv::Vec3d rvec = rvecs.at(i);
            cv::Vec3d tvec = tvecs.at(i);
            int id = markerIds.at(i);
            markers << new Marker(id, QVector3D(tvec[0], tvec[1], tvec[2]), QVector3D(rvec[0], rvec[1], rvec[2]), screenPos);
        }
        _d->refPlane.trackMarkers(1000.0 / 30.0, markers);
        qDeleteAll(markers);

        if (_d->refPlane.hasPlane()) {

            std::vector<cv::Point2f> projected;
            cv::Vec3d zero(0, 0, 0);
            std::vector<Point3f> corners;

            for (QVector3D pos : _d->refPlane.cornerPoints()) {
                cv::Vec3f tvec(pos.x(), pos.y(), pos.z());
                corners.push_back(tvec);
                annotate(view, true, zero, tvec, cameraMatrix, distCoeffs);
            }
            projectPoints(corners, zero, zero, cameraMatrix, distCoeffs, projected);

            std::vector<cv::Point2f> screenCorners;
            screenCorners.push_back(Point2f(1920, 1080));
            screenCorners.push_back(Point2f(0, 1080));
            screenCorners.push_back(Point2f(1920, 0));
            screenCorners.push_back(Point2f(0, 0));

            cv::Mat perspective = cv::getPerspectiveTransform(projected, screenCorners);
            cv::warpPerspective(view.clone(), view, perspective, cv::Size(1920, 1080));
        }

        for (size_t i = 0; i < rvecs.size(); ++i) {
            //            QPointF screenPos;
            //            for (const Point2f& p : markerCorners.at(i)) {
            //                screenPos.rx() += p.x;
            //                screenPos.ry() += p.y;
            //            }
            //            screenPos /= markerCorners.at(i).size();
            cv::Vec3d rvec = rvecs.at(i);
            cv::Vec3d tvec = tvecs.at(i);

            //            if (_d->refPlane.hasPlane()) {
            //                QVector3D projected = _d->refPlane.projectPoint(screenPos);
            //                tvec = cv::Vec3d(projected.x(), projected.y(), projected.z());
            //            }

            if (kallmanFilter) {
                int id = markerIds.at(i);
                foundIds.insert(id);
                KalmanTracker3D* tracker = _d->trackers.value(id);
                if (!tracker) {
                    _d->trackers.insert(id, tracker = new KalmanTracker3D(id == 46 ? KalmanTracker3D::movingTanksParams() : KalmanTracker3D::staticMarkerParams()));
                } else {
                    tracker->predict(1000.0 / 30.0);
                }
                tracker->update(QVector3D(tvec[0], tvec[1], tvec[2]), QVector3D(rvec[0], rvec[1], rvec[2]));
            }
            annotate(view, false, rvec, tvec, cameraMatrix, distCoeffs);
        }
        if (kallmanFilter) {
            for (auto it = _d->trackers.constBegin(); it != _d->trackers.constEnd(); ++it) {
                if (!foundIds.contains(it.key())) {
                    it.value()->predict(1000.0 / 30.0);
                }

                if (it.value()->hasPosition()) {
                    QVector3D pos = it.value()->position();
                    QVector3D rot = it.value()->rotation();
                    cv::Vec3d tvec(pos.x(), pos.y(), pos.z());
                    cv::Vec3d rvec(rot.x(), rot.y(), rot.z());

                    annotate(view, true, rvec, tvec, cameraMatrix, distCoeffs);
                }
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
                QVector3D position(tvecs.at(i)[0], tvecs.at(i)[1], tvecs.at(i)[2]);
                QVector3D rotation(rvecs.at(i)[0], rvecs.at(i)[1], rvecs.at(i)[2]);
                markers << new Marker(markerIds.at(i), position, rotation);
            }
            qSort(markers.begin(), markers.end(), [](Marker* a, Marker* b) -> bool { return a->id() < b->id(); });
            frame->setMarkers(markers);
        });
        result.waitForFinished();
    }
}
