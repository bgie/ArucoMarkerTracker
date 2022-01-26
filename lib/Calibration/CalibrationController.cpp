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
#include "CalibrationController.h"
#include "Aruco/Aruco.h"
#include "CameraCalibration.h"
#include "FramesCalibrationModel.h"
#include "Video/Frame.h"
#include "Video/Video.h"
#include <QDir>
#include <QSettings>

namespace {
const QString LOADPATH_KEY(QStringLiteral("CalibrationPath"));
const QString SAVEFILENAME_KEY(QStringLiteral("CalibrationFile"));
}

CalibrationController::CalibrationController(QObject* parent)
    : QObject(parent)
    , _video(new Video(this))
    , _frameIndex(-1)
    , _model(new FramesCalibrationModel(this))
    , _calibration(new CameraCalibration())
{
    QSettings settings;
    setLoadPath(settings.value(LOADPATH_KEY, QDir::homePath()).toString());
    setSaveFilename(settings.value(SAVEFILENAME_KEY, QStringLiteral("CameraCalibration.json")).toString());

    _model->setVideo(_video);
}

CalibrationController::~CalibrationController()
{
    delete _calibration;
}

Aruco* CalibrationController::aruco() const
{
    return _aruco;
}

void CalibrationController::setFrameIndex(int index)
{
    if (_frameIndex != index) {
        _frameIndex = index;

        if (_frameIndex >= 0 && _frameIndex < _video->frames().length()) {
            _image = _video->frames().at(index)->image();
            _calibration->annotateCheckerBoard(_image);
        } else {
            _image = QImage();
        }
        emit frameIndexChanged(_frameIndex);
        emit imageChanged(_image);
    }
}

void CalibrationController::setTotalError(QString totalError)
{
    if (_totalError == totalError)
        return;

    _totalError = totalError;
    emit totalErrorChanged(_totalError);
}

void CalibrationController::setLoadPath(QString loadPath)
{
    if (_loadPath == loadPath)
        return;

    _loadPath = loadPath;

    QSettings settings;
    settings.setValue(LOADPATH_KEY, _loadPath);

    _video->load(_loadPath);
    setFrameIndex(-1);

    emit loadPathChanged(_loadPath);
}

void CalibrationController::setSaveFilename(QString saveFilename)
{
    if (_saveFilename == saveFilename)
        return;

    _saveFilename = saveFilename;
    emit saveFilenameChanged(_saveFilename);
}

QString CalibrationController::loadPath() const
{
    return _loadPath;
}

QImage CalibrationController::image() const
{
    return _image;
}

int CalibrationController::frameIndex() const
{
    return _frameIndex;
}

FramesCalibrationModel* CalibrationController::model() const
{
    return _model;
}

void CalibrationController::calibrate()
{
    double error = _calibration->calibrateUsingVideo(_video->frames());
    if (_aruco) {
        _aruco->setCameraMatrix(_calibration->cameraMatrix(), _calibration->distCoeffs());
    }
    setCalibrationValues(_calibration->calibrationValues());
    setTotalError(QStringLiteral("RMS error: %1").arg(error));
}

QString CalibrationController::totalError() const
{
    return _totalError;
}

void CalibrationController::saveCalibration()
{
    _calibration->save(QDir(_loadPath).absoluteFilePath(_saveFilename));
}

void CalibrationController::loadCalibration()
{
    QString filename = QDir(_loadPath).absoluteFilePath(_saveFilename);
    if (QFile::exists(filename)) {
        _calibration->load(filename);
        if (_aruco) {
            _aruco->setCameraMatrix(_calibration->cameraMatrix(), _calibration->distCoeffs());
        }
        setCalibrationValues(_calibration->calibrationValues());
        setTotalError(QString());
    }
}

QString CalibrationController::calibrationValues() const
{
    return _calibrationValues;
}

QString CalibrationController::saveFilename() const
{
    return _saveFilename;
}

void CalibrationController::setCalibrationValues(QString calibrationValues)
{
    if (_calibrationValues == calibrationValues)
        return;

    _calibrationValues = calibrationValues;
    emit calibrationValuesChanged(_calibrationValues);
}

void CalibrationController::setAruco(Aruco* aruco)
{
    if (_aruco == aruco)
        return;

    _aruco = aruco;
    emit arucoChanged(_aruco);
}
