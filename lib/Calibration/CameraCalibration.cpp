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
#include "CameraCalibration.h"
#include <QtConcurrent/QtConcurrentMap>
#include <functional>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

const cv::Size PATTERN_SIZE(10, 7);

namespace {

struct imageCornersResult {
    std::vector<cv::Point2f> imageCorners;
    bool patternFound;
};

struct indexedImageCornersResult {
    int frameIndex;
    imageCornersResult result;
};

struct reduceResult {
    std::vector<std::vector<cv::Point2f>> imageCorners;
    std::vector<std::vector<cv::Point3f>> chessBoardCorners;
    std::vector<int> indices;
};

std::vector<cv::Point3f> chessBoardCorners()
{
    static std::vector<cv::Point3f> result = []() -> std::vector<cv::Point3f> {
        std::vector<cv::Point3f> r;
        for (int i = 0; i < PATTERN_SIZE.height; i++) {
            for (int j = 0; j < PATTERN_SIZE.width; j++) {
                r.push_back(cv::Point3f(float(i), float(j), 0.0f));
            }
        }
        return r;
    }();
    return result;
}

imageCornersResult getChessboardCorners(QImage image)
{
    imageCornersResult result;

    cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
    cv::Mat gray;
    cv::cvtColor(view, gray, cv::COLOR_RGB2GRAY);

    result.patternFound = cv::findChessboardCorners(
        gray,
        PATTERN_SIZE,
        result.imageCorners,
        cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

    if (result.patternFound) {
        cv::cornerSubPix(
            gray,
            result.imageCorners,
            cv::Size(11, 11),
            cv::Size(-1, -1),
            cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 50, 0.02));
    }
    return result;
}

void collectChessboardCorners(reduceResult& output, const indexedImageCornersResult& input)
{
    if (input.result.patternFound) {
        output.imageCorners.push_back(input.result.imageCorners);
        output.chessBoardCorners.push_back(chessBoardCorners());
        output.indices.push_back(input.frameIndex);
    }
}
}

struct CameraCalibrationData {
    cv::Size inputImageSize;
    cv::Mat mapX, mapY;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
};

CameraCalibration::CameraCalibration()
    : _d(new CameraCalibrationData())
{
}

CameraCalibration::~CameraCalibration()
{
}

void CameraCalibration::annotateCheckerBoard(QImage& image)
{
    if (image.size().isEmpty())
        return;

    imageCornersResult result = getChessboardCorners(image);

    cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
    cv::drawChessboardCorners(view, PATTERN_SIZE, cv::Mat(result.imageCorners), result.patternFound);
}

double CameraCalibration::calibrateUsingVideo(QList<Frame*> frames)
{
    double error = 0.0;
    _d->mapX = cv::Mat();
    _d->mapY = cv::Mat();

    if (frames.count()) {
        auto s = frames.first()->image().size();
        _d->inputImageSize = cv::Size(s.width(), s.height());

        QVector<int> range(frames.count()); // waiting for C++20 range class!
        for (int i = 0; i < range.size(); ++i)
            range[i] = i;

        std::function mapper = [=](int index) -> indexedImageCornersResult {
            indexedImageCornersResult indexedResult;
            indexedResult.frameIndex = index;
            indexedResult.result = getChessboardCorners(frames.at(index)->image());
            return indexedResult;
        };

        reduceResult result = QtConcurrent::blockingMappedReduced(
            range,
            mapper,
            collectChessboardCorners);

        std::vector<cv::Mat> rvecs, tvecs;
        std::vector<double> stdDeviationsIntrinsics;
        std::vector<double> stdDeviationsExtrinsics;
        std::vector<double> perViewErrors;

        error = calibrateCamera(
            result.chessBoardCorners,
            result.imageCorners,
            _d->inputImageSize,
            _d->cameraMatrix,
            _d->distCoeffs,
            rvecs, tvecs,
            stdDeviationsIntrinsics,
            stdDeviationsExtrinsics,
            perViewErrors,
            0);

        //        Q_ASSERT(perViewErrors.size() == result.indices.size());
        //        for (size_t i = 0; i < result.indices.size(); ++i) {
        //            frames.at(result.indices.at(i))->setChessBoardReprojectionError(QString::number(perViewErrors.at(i)));
        //        }

        cv::initUndistortRectifyMap(
            _d->cameraMatrix,
            _d->distCoeffs,
            cv::Mat(),
            cv::Mat(),
            _d->inputImageSize,
            CV_32FC1,
            _d->mapX,
            _d->mapY);
    }
    return error;
}

void CameraCalibration::save(QString filename)
{
    if (!_d->mapX.empty() && !_d->mapY.empty()) {
        cv::FileStorage file(filename.toStdString(), cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);
        file << "cameraMatrix" << _d->cameraMatrix;
        file << "distCoeffs" << _d->distCoeffs;
        file << "inputImageSize" << _d->inputImageSize;
    }
}

void CameraCalibration::load(QString filename)
{
    cv::FileStorage file(filename.toStdString(), cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
    file["cameraMatrix"] >> _d->cameraMatrix;
    file["distCoeffs"] >> _d->distCoeffs;

    file["inputImageSize"] >> _d->inputImageSize;
    cv::initUndistortRectifyMap(
        _d->cameraMatrix,
        _d->distCoeffs,
        cv::Mat(),
        cv::Mat(),
        _d->inputImageSize,
        CV_32FC1,
        _d->mapX,
        _d->mapY);
}

void CameraCalibration::undistortImage(QImage& image)
{
    if (!image.size().isEmpty() && !_d->mapX.empty() && !_d->mapY.empty()) {
        cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::Mat source = view.clone();
        cv::remap(source, view, _d->mapX, _d->mapY, cv::INTER_LINEAR);
    }
}

cv::Mat CameraCalibration::cameraMatrix() const
{
    return _d->cameraMatrix;
}

cv::Mat CameraCalibration::distCoeffs() const
{
    return _d->distCoeffs;
}

QString CameraCalibration::calibrationValues() const
{
    if (_d->cameraMatrix.empty() || _d->distCoeffs.empty())
        return QString();

    return QStringLiteral("Camera Matrix %1x%2, Distance Coeffients %3x%4")
        .arg(_d->cameraMatrix.size[0])
        .arg(_d->cameraMatrix.size[1])
        .arg(_d->distCoeffs.size[0])
        .arg(_d->distCoeffs.size[1]);
}
