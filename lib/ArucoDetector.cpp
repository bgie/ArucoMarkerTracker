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
#include <QHash>
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
    QHash<int, MarkerTracker*> trackers;
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
    qDeleteAll(_d->trackers.values());
}

void ArucoDetector::detectAruco(QImage& image, cv::Mat cameraMatrix, cv::Mat distCoeffs, bool kallmanFilter)
{
    if (image.size().isEmpty())
        return;

    std::vector<std::vector<Point2f>> markerCorners;
    std::vector<int> markerIds;
    cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
    detectMarkers(view, _d->dictionary, markerCorners, markerIds, _d->parameters, cv::noArray());

    // drawDetectedMarkers(view, markerCorners, markerIds);

    if (!cameraMatrix.empty() && !distCoeffs.empty()) {
        std::vector<cv::Vec3d> rvecs, tvecs;
        estimatePoseSingleMarkers(markerCorners, ARUCO_MARKER_SIZE_MM, cameraMatrix, distCoeffs, rvecs, tvecs);

        QSet<int> foundIds;

        for (size_t i = 0; i < rvecs.size(); ++i) {
            cv::Vec3d rvec = rvecs.at(i);
            cv::Vec3d tvec = tvecs.at(i);

            if (kallmanFilter) {
                int id = markerIds.at(i);
                foundIds.insert(id);
                MarkerTracker* tracker = _d->trackers.value(id);
                if (!tracker) {
                    _d->trackers.insert(id, tracker = new MarkerTracker(id == 46 ? MarkerTracker::movingTanksParams() : MarkerTracker::staticMarkerParams()));
                } else {
                    tracker->predict(1000.0 / 30.0);
                }
                tracker->update(QVector3D(tvec[0], tvec[1], tvec[2]), QVector3D(rvec[0], rvec[1], rvec[2]));
            }

            if (!kallmanFilter) {
                drawAxis(view, cameraMatrix, distCoeffs, rvec, tvec, ANNOTATE_AXIS_LENGTH);
            }
        }
        if (kallmanFilter) {
            for (auto it = _d->trackers.constBegin(); it != _d->trackers.constEnd(); ++it) {
                if (!foundIds.contains(it.key())) {
                    it.value()->predict(1000.0 / 30.0);
                    it.value()->updateNotFound();
                }

                if (it.value()->hasPosition()) {
                    QVector3D pos = it.value()->position();
                    QVector3D rot = it.value()->rotation();
                    cv::Vec3d tvec(pos.x(), pos.y(), pos.z());
                    cv::Vec3d rvec(rot.x(), rot.y(), rot.z());
                    drawAxis(view, cameraMatrix, distCoeffs, rvec, tvec, ANNOTATE_AXIS_LENGTH);
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
