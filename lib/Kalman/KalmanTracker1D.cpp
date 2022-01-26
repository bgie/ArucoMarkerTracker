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
#include "KalmanTracker1D.h"
#include <opencv2/video/tracking.hpp>

namespace {
const int stateSize = 2;
const int measSize = 1;
const int contrSize = 0;
}

KalmanTracker1D::Params::Params(double positionProcessNoiseCov, double velocityProcessNoiseCov, double measurementNoiseCov, double notUpdatedTimeoutInMsec)
    : positionProcessNoiseCov(positionProcessNoiseCov)
    , velocityProcessNoiseCov(velocityProcessNoiseCov)
    , measurementNoiseCov(measurementNoiseCov)
    , notUpdatedTimeoutInMsec(notUpdatedTimeoutInMsec)
{
}

struct KalmanTracker1D::Data {
    Data(const KalmanTracker1D::Params& p)
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
        //
        // Is already set to identity matrix in kf.init

        // Measure Matrix H
        kf.measurementMatrix.at<double>(0) = 1.0;

        // Process Noise Covariance Matrix Q
        // 1 0
        // 0 1
        kf.processNoiseCov.at<double>(0) = p.positionProcessNoiseCov;
        kf.processNoiseCov.at<double>(3) = p.velocityProcessNoiseCov;

        // Measures Noise Covariance Matrix R
        cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(p.measurementNoiseCov));
    }

    void update(const double position)
    {
        meas.at<double>(0) = position;

        if (notFoundCountDown <= 0) {
            state.at<double>(0) = meas.at<double>(0);
            state.at<double>(1) = 0;
            kf.statePost = state;
            setIdentity(kf.errorCovPost);
        } else {
            state = kf.correct(meas);
        }
        notFoundCountDown = p.notUpdatedTimeoutInMsec;
    }

    void predict(double elapsedMsec)
    {
        notFoundCountDown -= elapsedMsec;
        if (notFoundCountDown > 0) {
            kf.transitionMatrix.at<double>(1) = elapsedMsec;
            state = kf.predict();
        }
    }

    KalmanTracker1D::Params p;
    cv::KalmanFilter kf;
    cv::Mat state; // [x,v_x]
    cv::Mat meas; // [x]
    double notFoundCountDown;
};

KalmanTracker1D::KalmanTracker1D(const Params& p)
    : _d(new Data(p))
{
}

KalmanTracker1D::~KalmanTracker1D()
{
}

void KalmanTracker1D::update(double position)
{
    _d->update(position);
}

void KalmanTracker1D::predict(double elapsedMsec)
{
    _d->predict(elapsedMsec);
}

bool KalmanTracker1D::hasPosition() const
{
    return _d->notFoundCountDown > 0;
}

double KalmanTracker1D::position() const
{
    return _d->state.at<double>(0);
}

double KalmanTracker1D::velocity() const
{
    return _d->state.at<double>(1);
}
