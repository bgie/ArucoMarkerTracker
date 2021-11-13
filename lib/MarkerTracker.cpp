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
const int stateSize = 4;
const int measSize = 2;
const int contrSize = 0;
const unsigned int type = CV_32F;

struct MarkerTrackerData {
    MarkerTrackerData()
        : kf(stateSize, measSize, contrSize, type)
        , state(stateSize, 1, type)
        , meas(measSize, 1, type)
        , notFoundCountDown(0)
    {
        // Transition State Matrix A
        // Note: set dT at each processing step!
        // [ 1 0 dT 0  ]
        // [ 0 1 0  dT ]
        // [ 0 0 1  0  ]
        // [ 0 0 0  1  ]
        cv::setIdentity(kf.transitionMatrix);

        // Measure Matrix H
        // [ 1 0 0 0 ]
        // [ 0 1 0 0 ]
        kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
        kf.measurementMatrix.at<float>(0) = 1.0f;
        kf.measurementMatrix.at<float>(5) = 1.0f;

        // Process Noise Covariance Matrix Q
        // [ Ex   0   0     0     ]
        // [ 0    Ey  0     0     ]
        // [ 0    0   Ev_x  0     ]
        // [ 0    0   0     Ev_y  ]
        //cv::setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
        //        kf.processNoiseCov.at<float>(0) = 1e-2;
        //        kf.processNoiseCov.at<float>(5) = 1e-2;
        //        kf.processNoiseCov.at<float>(10) = 5.0f;
        //        kf.processNoiseCov.at<float>(15) = 5.0f;

        kf.processNoiseCov.at<float>(0) = kf.processNoiseCov.at<float>(5) = 1e+3;
        kf.processNoiseCov.at<float>(10) = kf.processNoiseCov.at<float>(15) = 1e+6;

        // Measures Noise Covariance Matrix R
        // cv::setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-1));
        cv::setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-6));
    }

    void update(const QVector3D& position)
    {
        meas.at<float>(0) = position.x();
        meas.at<float>(1) = position.y();

        if (notFoundCountDown == 0) {
            kf.errorCovPre.at<float>(0) = 10; // mm
            kf.errorCovPre.at<float>(5) = 10; // mm
            kf.errorCovPre.at<float>(10) = 1;
            kf.errorCovPre.at<float>(15) = 1;

            state.at<float>(0) = meas.at<float>(0);
            state.at<float>(1) = meas.at<float>(1);
            state.at<float>(2) = 0;
            state.at<float>(3) = 0;

            kf.statePost = state;
            notFoundCountDown = NOTFOUND_COUNTDOWN_START;
        } else {
            kf.correct(meas);
        }
        qDebug() << "Update x:" << state.at<float>(0) << " y:" << state.at<float>(1) << " vx:" << state.at<float>(2) << " vy:" << state.at<float>(3);
    }

    void updateNotFound()
    {
        updateNotFound();
        if (notFoundCountDown > 0) {
            notFoundCountDown--;
        }
    }

    void predict(float elapsedMsec)
    {
        if (notFoundCountDown > 0) {
            kf.transitionMatrix.at<float>(2) = elapsedMsec;
            kf.transitionMatrix.at<float>(7) = elapsedMsec;

            state = kf.predict();

            qDebug() << "Predict x:" << state.at<float>(0) << " y:" << state.at<float>(1) << " vx:" << state.at<float>(2) << " vy:" << state.at<float>(3);
        }
    }

    cv::KalmanFilter kf;
    cv::Mat state; // [x,y,v_x,v_y]
    cv::Mat meas; // [z_x,z_y]
    int notFoundCountDown;
    QVector3D filteredPosition;
};

MarkerTracker::MarkerTracker()
    : _d(new MarkerTrackerData())
{
}

MarkerTracker::~MarkerTracker()
{
}

void MarkerTracker::update(const QVector3D& position)
{
    _d->update(position);
}

void MarkerTracker::updateNotFound()
{
    _d->updateNotFound();
}

void MarkerTracker::predict(float elapsedMsec)
{
    _d->predict(elapsedMsec);
}

bool MarkerTracker::hasPosition() const
{
    return _d->notFoundCountDown > 0;
}

QVector3D MarkerTracker::position() const
{
    return QVector3D(_d->state.at<float>(0), _d->state.at<float>(1), 0);
}
