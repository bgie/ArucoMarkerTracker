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
#include "ReferencePlaneTracker.h"
#include "KalmanTracker3D.h"
#include "Marker.h"
#include <QDebug>
#include <opencv2/core.hpp>

struct ReferencePlaneTracker::Data {
    Data(QSet<int> referenceMarkerIds)
        : referenceMarkerIds(referenceMarkerIds)
        , hasPlane(false)
        , screenToWorldTransform(3, 3)
    {
    }

    ~Data()
    {
        qDeleteAll(trackers.values());
    }

    void trackReferenceMarkers(double elapsedMsecs, QList<Marker*> markers)
    {
        for (KalmanTracker3D* tracker : trackers.values()) {
            tracker->predict(elapsedMsecs);
        }

        for (Marker* m : markers) {
            int id = m->id();
            if (referenceMarkerIds.contains(id)) {
                KalmanTracker3D* tracker = trackers.value(id, nullptr);
                if (!tracker) {
                    tracker = new KalmanTracker3D(KalmanTracker3D::staticMarkerParams());
                    trackers.insert(id, tracker);
                }
                tracker->update(m->position(), m->rotation());
            }
        }
        int trackersWithPosition = 0;
        QList trackerList = trackers.values();
        for (KalmanTracker3D* tracker : trackerList) {
            if (tracker->hasPosition()) {
                ++trackersWithPosition;
            }
        }
        if (trackersWithPosition >= 3) {
            cv::Mat_<double> pointsXYm1(trackersWithPosition, 3, 1.0f);
            cv::Mat_<double> pointsWorldXYZ(trackersWithPosition, 3);
            int i = 0;
            //for (KalmanTracker3D* tracker : trackerList) {
            for (Marker* m : markers) {
                KalmanTracker3D* tracker = trackers.value(m->id(), nullptr);
                if (tracker && tracker->hasPosition()) {
                    QPointF sourcePos = m->screenPos();
                    QVector3D targetPos = tracker->position();
                    pointsXYm1.at<double>(i, 0) = sourcePos.x();
                    pointsXYm1.at<double>(i, 1) = sourcePos.y();
                    pointsWorldXYZ.at<double>(i, 0) = targetPos.x();
                    pointsWorldXYZ.at<double>(i, 1) = targetPos.y();
                    pointsWorldXYZ.at<double>(i, 2) = targetPos.z();
                    ++i;
                }
            }
            hasPlane = true;
            for (i = 0; i < 3; ++i) {
                hasPlane = hasPlane && cv::solve(pointsXYm1, pointsWorldXYZ.col(i), screenToWorldTransform.col(i), cv::DECOMP_QR);
            }

            cv::Mat_<double> pointsZ(trackersWithPosition, 1);
            cv::Mat_<double> planeABD;
            i = 0;
            for (KalmanTracker3D* tracker : trackerList) {
                if (tracker->hasPosition()) {
                    QVector3D pos = tracker->position();
                    pointsXYm1.at<double>(i, 0) = pos.x();
                    pointsXYm1.at<double>(i, 1) = pos.y();
                    pointsZ.at<double>(i, 0) = pos.z();
                    ++i;
                }
            }
            hasPlane = cv::solve(pointsXYm1, pointsZ, planeABD, cv::DECOMP_QR);
            if (hasPlane) {
                a = planeABD.at<double>(0, 0);
                b = planeABD.at<double>(1, 0);
                d = planeABD.at<double>(2, 0);
                len = sqrt(a * a + b * b + 1);
                normal = cv::Vec3d(a / len, b / len, -1 / len);
                scalar = d / len;
            }
        } else {
            hasPlane = false;
        }
    }

    QVector3D centerPoint() const
    {
        return QVector3D(0, 0, d);
    }

    QList<QVector3D> cornerPoints() const
    {
        QList<QVector3D> result;
        QVector3D c = centerPoint();
        QVector3D unitX = QVector3D(normal[2], 0, -normal[0]).normalized();
        QVector3D unitY = QVector3D(0, normal[2], -normal[1]).normalized();

        float sx = 860, sy = 540;
        result << c - sx * unitX - sy * unitY;
        result << c + sx * unitX - sy * unitY;
        result << c - sx * unitX + sy * unitY;
        result << c + sx * unitX + sy * unitY;
        return result;
    }

    QVector3D projectPoint(const QPointF& input) const
    {
        //        cv::Vec3d in(input.x(), input.y(), input.z());
        //        double distance = in.dot(normal) + scalar;
        //        cv::Vec3d out = in - normal * distance;
        //        return QVector3D(out[0], out[1], out[2]);
        cv::Mat_<double> in(1, 3, 1);
        in.at<double>(0, 0) = input.x();
        in.at<double>(0, 1) = input.y();
        cv::Mat out = in * screenToWorldTransform;
        return QVector3D(out.at<double>(0, 0), out.at<double>(0, 1), out.at<double>(0, 2));
    }

    QSet<int> referenceMarkerIds;
    QHash<int, KalmanTracker3D*> trackers;
    bool hasPlane;
    double a, b, d, len;
    cv::Vec3d normal;
    double scalar;
    cv::Mat_<double> screenToWorldTransform;
};

ReferencePlaneTracker::ReferencePlaneTracker(QSet<int> referenceMarkerIds)
    : _d(new Data(referenceMarkerIds))
{
}

ReferencePlaneTracker::~ReferencePlaneTracker()
{
}

bool ReferencePlaneTracker::hasPlane() const
{
    return _d->hasPlane;
}

void ReferencePlaneTracker::trackMarkers(double elapsedMsecs, QList<Marker*> markers)
{
    _d->trackReferenceMarkers(elapsedMsecs, markers);
}

QVector3D ReferencePlaneTracker::centerPoint() const
{
    return _d->centerPoint();
}

QList<QVector3D> ReferencePlaneTracker::cornerPoints() const
{
    return _d->cornerPoints();
}

QVector3D ReferencePlaneTracker::projectPoint(const QPointF& input) const
{
    return _d->projectPoint(input);
}
