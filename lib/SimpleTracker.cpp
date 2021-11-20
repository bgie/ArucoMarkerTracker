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
#include "SimpleTracker.h"
#include <QDebug>
#include <opencv2/video/tracking.hpp>

const int NOTFOUND_COUNTDOWN_START = 90;
const int stateSize = 2;
const int measSize = 1;
const int contrSize = 0;

SimpleTracker::Params::Params()
    : positionProcessNoiseCov(1)
    , velocityProcessNoiseCov(1)
    , measurementNoiseCov(1)
{
}

struct SimpleTrackerData {
    SimpleTrackerData(const SimpleTracker::Params& p)
        : p(p)
        , kf(stateSize, measSize, contrSize, CV_64F)
        , state(stateSize, 1, CV_64F)
        , meas(measSize, 1, CV_64F)
        , notFoundCountDown(0)
    {
        // Transition State Matrix A
        //   p v
        // p 1 dT
        // v 0 1
        // Note: set dT at each processing step!
        cv::setIdentity(kf.transitionMatrix);

        // Measure Matrix H
        kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, CV_64F);
        kf.measurementMatrix.at<double>(0) = 1.0;

        // Process Noise Covariance Matrix Q
        // 1 0
        // 0 1
        cv::setIdentity(kf.processNoiseCov);
        kf.processNoiseCov.at<double>(0) = p.positionProcessNoiseCov;
        kf.processNoiseCov.at<double>(3) = p.velocityProcessNoiseCov;

        // Measures Noise Covariance Matrix R
        cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(p.measurementNoiseCov));

        cv::setIdentity(kf.errorCovPost);
    }

    void update(const double position)
    {
        meas.at<double>(0) = position;

        if (notFoundCountDown == 0) {
            state.at<double>(0) = meas.at<double>(0);
            state.at<double>(1) = 0;

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
            kf.transitionMatrix.at<double>(1) = elapsedMsec;
            state = kf.predict();
        }
    }

    SimpleTracker::Params p;
    cv::KalmanFilter kf;
    cv::Mat state; // [x,v_x]
    cv::Mat meas; // [x]
    int notFoundCountDown;
};

SimpleTracker::SimpleTracker(const Params& p)
    : _d(new SimpleTrackerData(p))
{
}

SimpleTracker::~SimpleTracker()
{
}

void SimpleTracker::update(double position)
{
    _d->update(position);
}

void SimpleTracker::updateNotFound()
{
    _d->updateNotFound();
}

void SimpleTracker::predict(double elapsedMsec)
{
    _d->predict(elapsedMsec);
}

bool SimpleTracker::hasPosition() const
{
    return _d->notFoundCountDown > 0;
}

double SimpleTracker::position() const
{
    return _d->state.at<double>(0);
}

double SimpleTracker::velocity() const
{
    return _d->state.at<double>(1);
}
