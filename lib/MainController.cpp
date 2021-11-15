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
#include "MainController.h"
#include "Camera.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QTimer>

namespace {
const QString VIDEODEVICE_KEY(QStringLiteral("VideoDevice"));
const QString EXPOSURE_KEY(QStringLiteral("Exposure"));
const QString VIDEOFORMATINDEX_KEY(QStringLiteral("VideoFormatIndex"));
const QString SAVEPATH_KEY(QStringLiteral("SavePath"));
const QString SKIPSAVINGFRAMES_KEY(QStringLiteral("SkipSavingFrames"));
const QString LOADPATH_KEY(QStringLiteral("LoadPath"));
const QString REPLAYFPS_KEY(QStringLiteral("ReplayFps"));
const QString VIDEOJSON(QStringLiteral("Video.json"));
}

MainController::MainController(QObject* parent)
    : QObject(parent)
    , _connectPossible(false)
    , _currentVideoFormatIndex(-1)
    , _exposure(0)
    , _canCameraStream(false)
    , _isCameraStreaming(false)
    , _imageChanged(false)
    , _fps(0)
    , _framesCounter(0)
    , _saveallframesEnabled(false)
    , _skipSavingFrames(0)
    , _skipSavingFramesCounter(0)
    , _canReplayStream(false)
    , _isReplayStreaming(false)
    , _detectCheckerBoardEnabled(false)
    , _cameraCalibration(new CameraCalibration())
    , _applyCalibrationEnabled(false)
    , _arucoDetector(new ArucoDetector())
    , _detectArucoEnabled(false)
    , _kallmanFilterEnabled(false)
    , _frameIndex(-1)
{
    QSettings settings;
    setVideoDevice(settings.value(VIDEODEVICE_KEY, QStringLiteral("/dev/video0")).toString());
    setExposure(settings.value(EXPOSURE_KEY, 100).toInt());
    setSavePath(settings.value(SAVEPATH_KEY, QDir::homePath()).toString());
    setSkipSavingFrames(settings.value(SKIPSAVINGFRAMES_KEY, 0).toInt());
    setLoadPath(settings.value(LOADPATH_KEY, QDir::homePath()).toString());
    setReplayFps(settings.value(REPLAYFPS_KEY, QStringLiteral("10")).toString());

    _elapsedTime.start();
    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &MainController::updateFps);
    timer->setInterval(500);
    timer->setSingleShot(false);
    timer->start();

    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &MainController::processImage);
    timer->setInterval(0);
    timer->setSingleShot(false);
    timer->start();

    QObject::connect(&_replayTimer, &ReplayTimer::playFrame, this, &MainController::setFrameIndex);

    setFrameIndex(-1);
}

MainController::~MainController()
{
}

QString MainController::videoDevice() const
{
    return _videoDevice;
}

void MainController::connect()
{
    if (_connectPossible && (_camera.isNull() || _camera->deviceName() != _videoDevice)) {
        QSettings settings;
        settings.setValue(VIDEODEVICE_KEY, _videoDevice);

        _camera.reset(new Camera(_videoDevice));
        QObject::connect(_camera.data(), &Camera::frameRead, this, &MainController::cameraImageReceived);

        auto formats = _camera->videoFormats();
        auto formatIndex = settings.value(VIDEOFORMATINDEX_KEY, formats.count() - 1).toInt();
        setVideoFormats(formats);
        setCurrentVideoFormatIndex(formatIndex);
        _camera->setExposure(_exposure);
    }
}

void MainController::startCameraStream()
{
    if (_canCameraStream && !_camera.isNull() && !_isCameraStreaming && !_isReplayStreaming) {
        _camera->startStream();
        setIsCameraStreaming(true);
    }
}

void MainController::stopCameraStream()
{
    if (_isCameraStreaming) {
        _camera->stopStream();
        setIsCameraStreaming(false);
    }
}

bool MainController::connectPossible() const
{
    return _connectPossible;
}

QStringList MainController::videoFormats() const
{
    return _videoFormats;
}

int MainController::currentVideoFormatIndex() const
{
    return _currentVideoFormatIndex;
}

bool MainController::canCameraStream() const
{
    return _canCameraStream;
}

bool MainController::isCameraStreaming() const
{
    return _isCameraStreaming;
}

QImage MainController::image() const
{
    return _processedImage;
}

void MainController::setVideoDevice(QString videoDevice)
{
    if (_videoDevice == videoDevice)
        return;

    _videoDevice = videoDevice;

    setConnectPossible(Camera::isValidDevice(_videoDevice));

    emit videoDeviceChanged(_videoDevice);
}

void MainController::setConnectPossible(bool connectPossible)
{
    if (_connectPossible == connectPossible)
        return;

    _connectPossible = connectPossible;
    emit connectPossibleChanged(_connectPossible);
}

void MainController::setVideoFormats(QStringList videoFormats)
{
    if (_videoFormats == videoFormats)
        return;

    _videoFormats = videoFormats;
    emit videoFormatsChanged(_videoFormats);
}

void MainController::setCurrentVideoFormatIndex(int currentVideoFormatIndex)
{
    if (_currentVideoFormatIndex != currentVideoFormatIndex) {
        _currentVideoFormatIndex = currentVideoFormatIndex;

        if (!_camera.isNull()) {
            _camera->setVideoFormatIndex(_currentVideoFormatIndex);
            QSettings settings;
            settings.setValue(VIDEOFORMATINDEX_KEY, _currentVideoFormatIndex);
        }

        emit currentVideoFormatIndexChanged(_currentVideoFormatIndex);
    }

    setCanCameraStream(_camera.isNull() ? false : _camera->canStream());
}

void MainController::setCanCameraStream(bool canCameraStream)
{
    if (_canCameraStream == canCameraStream)
        return;

    _canCameraStream = canCameraStream;
    emit canCameraStreamChanged(_canCameraStream);
}

void MainController::setIsCameraStreaming(bool isCameraStreaming)
{
    if (_isCameraStreaming == isCameraStreaming)
        return;

    _isCameraStreaming = isCameraStreaming;
    emit isCameraStreamingChanged(_isCameraStreaming);
}

void MainController::cameraImageReceived(QImage image)
{
    if (_skipSavingFramesCounter == 0) {
        setImage(image);
        _saver.saveImage(image);

        _skipSavingFramesCounter = _skipSavingFrames;
    } else {
        _skipSavingFramesCounter--;
    }
}

void MainController::setImage(QImage image)
{
    if (_image == image)
        return;

    _image = image;
    _imageChanged = true;
}

void MainController::setFrameIndex(int index)
{
    if (_frameIndex != index) {
        _frameIndex = index;

        if (_frameIndex >= 0 && _frameIndex < _video.frames().length()) {
            setImage(_video.frames().at(index)->image());
        }
        emit frameIndexChanged(_frameIndex);
        emit frameChanged(frame());
    }
    updateFrameIndexString();
}

void MainController::setFps(qreal fps)
{
    if (qFuzzyCompare(_fps, fps))
        return;

    _fps = fps;

    emit fpsChanged(_fps);
}

void MainController::setSavePath(QString savePath)
{
    if (_savePath == savePath)
        return;

    _savePath = savePath;

    QSettings settings;
    settings.setValue(SAVEPATH_KEY, _savePath);

    _saver.setPath(_savePath);

    emit savePathChanged(_savePath);
}

void MainController::setSaveallframesEnabled(bool saveallframesEnabled)
{
    if (_saveallframesEnabled == saveallframesEnabled)
        return;

    _saveallframesEnabled = saveallframesEnabled;

    _saver.setEnabled(_saveallframesEnabled);

    emit saveallframesEnabledChanged(_saveallframesEnabled);
}

void MainController::setLoadPath(QString loadPath)
{
    if (_loadPath == loadPath)
        return;

    _loadPath = loadPath;

    QSettings settings;
    settings.setValue(LOADPATH_KEY, _loadPath);

    _video.load(_loadPath);

    _replayTimer.setFrameCount(_video.framesQObjects().count());
    setCanReplayStream(_replayTimer.canReplay());
    setFrameIndex(-1);

    emit loadPathChanged(_loadPath);
}

void MainController::setReplayFps(QString replayFps)
{
    if (_replayFps == replayFps)
        return;

    _replayFps = replayFps;

    QSettings settings;
    settings.setValue(REPLAYFPS_KEY, _replayFps);

    _replayTimer.setFps(_replayFps.toInt());
    setCanReplayStream(_replayTimer.canReplay());

    emit replayFpsChanged(_replayFps);
}

void MainController::setCanReplayStream(bool canReplayStream)
{
    if (_canReplayStream == canReplayStream)
        return;

    _canReplayStream = canReplayStream;
    emit canReplayStreamChanged(_canReplayStream);
}

void MainController::setIsReplayStreaming(bool isReplayStreaming)
{
    if (_isReplayStreaming == isReplayStreaming)
        return;

    _isReplayStreaming = isReplayStreaming;
    emit isReplayStreamingChanged(_isReplayStreaming);
}

void MainController::setDetectCheckerBoardEnabled(bool detectCheckerBoardEnabled)
{
    if (_detectCheckerBoardEnabled == detectCheckerBoardEnabled)
        return;

    _detectCheckerBoardEnabled = detectCheckerBoardEnabled;
    _imageChanged = true;
    emit detectCheckerBoardEnabledChanged(_detectCheckerBoardEnabled);
}

void MainController::setCalibrationResultString(QString calibrationResultString)
{
    if (_calibrationResultString == calibrationResultString)
        return;

    _calibrationResultString = calibrationResultString;
    emit calibrationResultStringChanged(_calibrationResultString);
}

void MainController::setApplyCalibrationEnabled(bool applyCalibrationEnabled)
{
    if (_applyCalibrationEnabled == applyCalibrationEnabled)
        return;

    _applyCalibrationEnabled = applyCalibrationEnabled;
    _imageChanged = true;
    emit applyCalibrationEnabledChanged(_applyCalibrationEnabled);
}

void MainController::setDetectArucoEnabled(bool detectArucoEnabled)
{
    if (_detectArucoEnabled == detectArucoEnabled)
        return;

    _detectArucoEnabled = detectArucoEnabled;
    _imageChanged = true;
    emit detectArucoEnabledChanged(_detectArucoEnabled);
}

void MainController::setFrameIndexString(QString frameIndexString)
{
    if (_frameIndexString == frameIndexString)
        return;

    _frameIndexString = frameIndexString;
    emit frameIndexStringChanged(_frameIndexString);
}

void MainController::setFrameFileName(QString frameFileName)
{
    if (_frameFileName == frameFileName)
        return;

    _frameFileName = frameFileName;
    emit frameFileNameChanged(_frameFileName);
}

void MainController::setKallmanFilterEnabled(bool kallmanFilterEnabled)
{
    if (_kallmanFilterEnabled == kallmanFilterEnabled)
        return;

    _kallmanFilterEnabled = kallmanFilterEnabled;
    emit kallmanFilterEnabledChanged(_kallmanFilterEnabled);
}

void MainController::setSkipSavingFrames(int skipSavingFrames)
{
    if (_skipSavingFrames == skipSavingFrames)
        return;

    _skipSavingFrames = skipSavingFrames;
    _skipSavingFramesCounter = _skipSavingFrames;

    QSettings settings;
    settings.setValue(SKIPSAVINGFRAMES_KEY, _skipSavingFrames);

    emit skipSavingFramesChanged(_skipSavingFrames);
}

void MainController::setExposure(int exposure)
{
    if (_exposure == exposure)
        return;

    _exposure = exposure;

    QSettings settings;
    settings.setValue(EXPOSURE_KEY, _exposure);

    if (_camera) {
        _camera->setExposure(_exposure);
    }

    emit exposureChanged(_exposure);
}

void MainController::updateFps()
{
    auto elapsed = _elapsedTime.restart();
    qreal newfps = _framesCounter * 1000.0 / elapsed;
    _framesCounter = 0;
    setFps(newfps);
}

void MainController::processImage()
{
    if (_imageChanged) {
        _processedImage = _image.copy();

        if (_detectCheckerBoardEnabled) {
            _cameraCalibration->annotateCheckerBoard(_processedImage);
        }
        if (_applyCalibrationEnabled) {
            _cameraCalibration->undistortImage(_processedImage);
        }

        if (_detectArucoEnabled) {
            _arucoDetector->detectAruco(_processedImage, _cameraCalibration->cameraMatrix(), _cameraCalibration->distCoeffs(), _kallmanFilterEnabled);
        }

        _framesCounter++;
        _imageChanged = false;
        emit imageChanged(_processedImage);
    }
}

void MainController::updateFrameIndexString()
{
    if (_video.framesQObjects().count() > 0 && _frameIndex >= 0) {
        setFrameIndexString(QStringLiteral("%1/%2").arg(_frameIndex + 1).arg(_video.frames().count()));
        setFrameFileName(_video.frames().at(_frameIndex)->fileName());
    } else {
        setFrameIndexString(QStringLiteral("-"));
        setFrameFileName(QString());
    }
}

qreal MainController::fps() const
{
    return _fps;
}

QString MainController::savePath() const
{
    return _savePath;
}

bool MainController::saveallframesEnabled() const
{
    return _saveallframesEnabled;
}

void MainController::saveSingleFrame()
{
    if (!_saveallframesEnabled && !_image.isNull()) {
        _saver.saveSingleImage(_image);
    }
}

QString MainController::loadPath() const
{
    return _loadPath;
}

void MainController::loadVideo()
{
    _video.load(_loadPath, VIDEOJSON);
}

void MainController::saveVideo()
{
    _video.save(_loadPath, VIDEOJSON);
}

QString MainController::replayFps() const
{
    return _replayFps;
}

bool MainController::canReplayStream() const
{
    return _canReplayStream;
}

void MainController::startReplayStream()
{
    if (_replayTimer.canReplay() && !_isCameraStreaming && !_isReplayStreaming) {
        _replayTimer.setAutoPlayEnabled(true);
        setIsReplayStreaming(true);
    }
}

void MainController::stopReplayStream()
{
    if (_isReplayStreaming) {
        _replayTimer.setAutoPlayEnabled(false);
        setIsReplayStreaming(false);
    }
}

bool MainController::isReplayStreaming() const
{
    return _isReplayStreaming;
}

bool MainController::detectCheckerBoardEnabled() const
{
    return _detectCheckerBoardEnabled;
}

void MainController::calibrateUsingAll()
{
    double error = _cameraCalibration->calibrateUsingVideo(_video.frames());
    setCalibrationResultString(QStringLiteral("RMS error: %1").arg(error));
}

void MainController::saveCalibration()
{
    _cameraCalibration->save(QDir(_loadPath).absoluteFilePath("CameraCalibration.json"));
}

void MainController::loadCalibration()
{
    QString filename = QDir(_loadPath).absoluteFilePath("CameraCalibration.json");
    if (QFile::exists(filename))
        _cameraCalibration->load(filename);
}

void MainController::detectArucoInAll()
{
    _arucoDetector->detectAllMarkers(_video.frames(), _cameraCalibration->cameraMatrix(), _cameraCalibration->distCoeffs());
}

bool MainController::kallmanFilterEnabled() const
{
    return _kallmanFilterEnabled;
}

Frame* MainController::frame() const
{
    return _video.frames().value(_frameIndex, nullptr);
}

QString MainController::frameFileName() const
{
    return _frameFileName;
}

int MainController::skipSavingFrames() const
{
    return _skipSavingFrames;
}

int MainController::exposure() const
{
    return _exposure;
}

QString MainController::calibrationResultString() const
{
    return _calibrationResultString;
}

bool MainController::applyCalibrationEnabled() const
{
    return _applyCalibrationEnabled;
}

bool MainController::detectArucoEnabled() const
{
    return _detectArucoEnabled;
}

Video* MainController::video()
{
    return &_video;
}

int MainController::frameIndex() const
{
    return _frameIndex;
}

QString MainController::frameIndexString() const
{
    return _frameIndexString;
}

void MainController::frameFirst()
{
    if (_video.framesQObjects().count()) {
        setFrameIndex(0);
    }
}

void MainController::framePrevious()
{
    if (_video.framesQObjects().count()) {
        setFrameIndex((_frameIndex + _video.framesQObjects().count() - 1) % _video.framesQObjects().count());
    }
}

void MainController::frameNext()
{
    if (_video.framesQObjects().count()) {
        setFrameIndex((_frameIndex + 1) % _video.framesQObjects().count());
    }
}

void MainController::deleteFrame()
{
    if (_video.framesQObjects().count() > 0 && _frameIndex >= 0) {
        QString fullFilename = QDir(_loadPath).absoluteFilePath(_video.frames().at(_frameIndex)->fileName());
        if (QFile::exists(fullFilename)) {
            QFile::remove(fullFilename);

            _video.load(_loadPath);

            _replayTimer.setFrameCount(_video.framesQObjects().count());
            setCanReplayStream(_replayTimer.canReplay());
            setFrameIndex(qMin(_frameIndex, _video.framesQObjects().count() - 1));
        }
    }
}
