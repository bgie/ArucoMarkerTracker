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
#include "MarkerTracker.h"
#include <QDebug>
#include <opencv2/video/tracking.hpp>

const int NOTFOUND_COUNTDOWN_START = 90;
const int stateSize = 6;
const int measSize = 3;
const int contrSize = 0;

// QVector(10, 0, 20480, 8192, 0.21875, 48, 64, 28, 64, 448, 32, 448)

MarkerTracker::Params::Params()
    : positionXYProcessNoiseCov(10)
    , positionZProcessNoiseCov(0)
    , velocityXYProcessNoiseCov(20480)
    , velocityZProcessNoiseCov(8192)
    , measurementXYNoiseCov(0.21875)
    , measurementZNoiseCov(48)
    , positionXYerrorCovPost(64)
    , positionZerrorCovPost(28)
    , velocityXYerrorCovPost(64)
    , velocityZerrorCovPost(448)
    , posVelXYProcessNoiseCov(32)
    , posVelZProcessNoiseCov(448)
{
}

struct MarkerTrackerData {
    MarkerTrackerData(const MarkerTracker::Params& p)
        : p(p)
        , kf(stateSize, measSize, contrSize, CV_64F)
        , state(stateSize, 1, CV_64F)
        , meas(measSize, 1, CV_64F)
        , notFoundCountDown(0)
    {
        // Transition State Matrix A
        // Note: set dT at each processing step!
        // [ 1 0 0 dT 0 0 ]
        // [ 0 1 0 0 dT 0 ]
        // [ 0 0 1 0 0 dT ]
        // [ 0 0 0 1 0  0 ]
        // [ 0 0 0 0 1  0 ]
        // [ 0 0 0 0 0  1 ]
        cv::setIdentity(kf.transitionMatrix);

        // Measure Matrix H
        // [ 1 0 0 0 0 0 ]
        // [ 0 1 0 0 0 0 ]
        // [ 0 0 1 0 0 0 ]
        kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, CV_64F);
        kf.measurementMatrix.at<double>(0) = 1.0;
        kf.measurementMatrix.at<double>(7) = 1.0;
        kf.measurementMatrix.at<double>(14) = 1.0;

        // Process Noise Covariance Matrix Q
        //       p_x p_y p_z v_x  v_y   v_z
        // p_x [ Ex  0   0   *     0     0    ]
        // p_y [ 0   Ey  0   0     *     0    ]
        // p_z [ 0   0   Ez  0     0     *    ]
        // v_x [ *   0   0   Ev_x  0     0    ]
        // v_y [ 0   *   0   0     Ev_y  0    ]
        // v_z [ 0   0   *   0     0     Ev_z ]
        cv::setIdentity(kf.processNoiseCov);
        kf.processNoiseCov.at<double>(0) = kf.processNoiseCov.at<double>(7) = p.positionXYProcessNoiseCov;
        kf.processNoiseCov.at<double>(14) = p.positionZProcessNoiseCov;
        kf.processNoiseCov.at<double>(21) = kf.processNoiseCov.at<double>(28) = p.velocityXYProcessNoiseCov;
        kf.processNoiseCov.at<double>(35) = p.velocityZProcessNoiseCov;
        kf.processNoiseCov.at<double>(3) = kf.processNoiseCov.at<double>(10) = p.posVelXYProcessNoiseCov;
        kf.processNoiseCov.at<double>(18) = kf.processNoiseCov.at<double>(25) = p.posVelXYProcessNoiseCov;
        kf.processNoiseCov.at<double>(17) = p.posVelZProcessNoiseCov;
        kf.processNoiseCov.at<double>(32) = p.posVelZProcessNoiseCov;

        // Measures Noise Covariance Matrix R
        cv::setIdentity(kf.measurementNoiseCov);
        kf.measurementNoiseCov.at<double>(0) = kf.measurementNoiseCov.at<double>(5) = p.measurementXYNoiseCov;
        kf.measurementNoiseCov.at<double>(10) = p.measurementZNoiseCov;

        // Posteriori Error Covariance Matrix
        cv::setIdentity(kf.errorCovPost);
        kf.errorCovPost.at<double>(0) = kf.errorCovPost.at<double>(7) = p.positionXYerrorCovPost;
        kf.errorCovPost.at<double>(14) = p.positionZerrorCovPost;
        kf.errorCovPost.at<double>(21) = kf.errorCovPost.at<double>(28) = p.velocityXYerrorCovPost;
        kf.errorCovPost.at<double>(35) = p.velocityZerrorCovPost;
    }

    void update(const QVector3D& position)
    {
        meas.at<double>(0) = position.x();
        meas.at<double>(1) = position.y();
        meas.at<double>(2) = position.z();

        if (notFoundCountDown == 0) {
            state.at<double>(0) = meas.at<double>(0);
            state.at<double>(1) = meas.at<double>(1);
            state.at<double>(2) = meas.at<double>(2);
            state.at<double>(3) = 0;
            state.at<double>(4) = 0;
            state.at<double>(5) = 0;

            kf.statePost = state;
            notFoundCountDown = NOTFOUND_COUNTDOWN_START;
        } else {
            state = kf.correct(meas);
        }
    }

    void updateNotFound()
    {
        if (notFoundCountDown > 0) {
            notFoundCountDown--;
        }
    }

    void predict(double elapsedMsec)
    {
        if (notFoundCountDown > 0) {
            kf.transitionMatrix.at<double>(3) = elapsedMsec;
            kf.transitionMatrix.at<double>(10) = elapsedMsec;
            kf.transitionMatrix.at<double>(17) = elapsedMsec;

            state = kf.predict();
        }
    }

    MarkerTracker::Params p;
    cv::KalmanFilter kf;
    cv::Mat state; // [x,y,z,v_x,v_y,v_z]
    cv::Mat meas; // [x,y,z]
    int notFoundCountDown;
    QVector3D rotation;
};

MarkerTracker::MarkerTracker(const Params& p)
    : _d(new MarkerTrackerData(p))
{
}

MarkerTracker::~MarkerTracker()
{
}

void MarkerTracker::update(const QVector3D& position, const QVector3D& rotation)
{
    _d->update(position);
    _d->rotation = rotation;
}

void MarkerTracker::updateNotFound()
{
    _d->updateNotFound();
}

void MarkerTracker::predict(double elapsedMsec)
{
    _d->predict(elapsedMsec);
}

bool MarkerTracker::hasPosition() const
{
    return _d->notFoundCountDown > 0;
}

QVector3D MarkerTracker::position() const
{
    return QVector3D(_d->state.at<double>(0), _d->state.at<double>(1), _d->state.at<double>(2));
}

QVector3D MarkerTracker::rotation() const
{
    return _d->rotation;
}
