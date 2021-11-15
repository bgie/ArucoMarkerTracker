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
#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "ArucoDetector.h"
#include "CameraCalibration.h"
#include "ImageSaver.h"
#include "ReplayTimer.h"
#include "Video.h"
#include <QElapsedTimer>
#include <QImage>
#include <QObject>

class Camera;

class MainController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString videoDevice READ videoDevice WRITE setVideoDevice NOTIFY videoDeviceChanged)
    Q_PROPERTY(bool connectPossible READ connectPossible WRITE setConnectPossible NOTIFY connectPossibleChanged)
    Q_PROPERTY(QStringList videoFormats READ videoFormats WRITE setVideoFormats NOTIFY videoFormatsChanged)
    Q_PROPERTY(int currentVideoFormatIndex READ currentVideoFormatIndex WRITE setCurrentVideoFormatIndex NOTIFY currentVideoFormatIndexChanged)
    Q_PROPERTY(int exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(bool canCameraStream READ canCameraStream WRITE setCanCameraStream NOTIFY canCameraStreamChanged)
    Q_PROPERTY(bool isCameraStreaming READ isCameraStreaming WRITE setIsCameraStreaming NOTIFY isCameraStreamingChanged)
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(qreal fps READ fps WRITE setFps NOTIFY fpsChanged)
    Q_PROPERTY(QString savePath READ savePath WRITE setSavePath NOTIFY savePathChanged)
    Q_PROPERTY(bool saveallframesEnabled READ saveallframesEnabled WRITE setSaveallframesEnabled NOTIFY saveallframesEnabledChanged)
    Q_PROPERTY(int skipSavingFrames READ skipSavingFrames WRITE setSkipSavingFrames NOTIFY skipSavingFramesChanged)
    Q_PROPERTY(QString loadPath READ loadPath WRITE setLoadPath NOTIFY loadPathChanged)
    Q_PROPERTY(QString replayFps READ replayFps WRITE setReplayFps NOTIFY replayFpsChanged)
    Q_PROPERTY(bool canReplayStream READ canReplayStream WRITE setCanReplayStream NOTIFY canReplayStreamChanged)
    Q_PROPERTY(bool isReplayStreaming READ isReplayStreaming WRITE setIsReplayStreaming NOTIFY isReplayStreamingChanged)
    Q_PROPERTY(bool detectCheckerBoardEnabled READ detectCheckerBoardEnabled WRITE setDetectCheckerBoardEnabled NOTIFY detectCheckerBoardEnabledChanged)
    Q_PROPERTY(QString calibrationResultString READ calibrationResultString WRITE setCalibrationResultString NOTIFY calibrationResultStringChanged)
    Q_PROPERTY(bool applyCalibrationEnabled READ applyCalibrationEnabled WRITE setApplyCalibrationEnabled NOTIFY applyCalibrationEnabledChanged)
    Q_PROPERTY(bool detectArucoEnabled READ detectArucoEnabled WRITE setDetectArucoEnabled NOTIFY detectArucoEnabledChanged)
    Q_PROPERTY(bool kallmanFilterEnabled READ kallmanFilterEnabled WRITE setKallmanFilterEnabled NOTIFY kallmanFilterEnabledChanged)
    Q_PROPERTY(Video* video READ video CONSTANT)
    Q_PROPERTY(int frameIndex READ frameIndex WRITE setFrameIndex NOTIFY frameIndexChanged)
    Q_PROPERTY(QString frameIndexString READ frameIndexString WRITE setFrameIndexString NOTIFY frameIndexStringChanged)
    Q_PROPERTY(QString frameFileName READ frameFileName WRITE setFrameFileName NOTIFY frameFileNameChanged)
    Q_PROPERTY(Frame* frame READ frame NOTIFY frameChanged)

public:
    explicit MainController(QObject* parent = nullptr);
    virtual ~MainController();

    QString videoDevice() const;
    bool connectPossible() const;
    QStringList videoFormats() const;
    int currentVideoFormatIndex() const;
    int exposure() const;
    Q_INVOKABLE void connect();
    bool canCameraStream() const;
    Q_INVOKABLE void startCameraStream();
    Q_INVOKABLE void stopCameraStream();
    bool isCameraStreaming() const;

    QImage image() const;
    qreal fps() const;

    QString savePath() const;
    bool saveallframesEnabled() const;
    int skipSavingFrames() const;
    Q_INVOKABLE void saveSingleFrame();

    QString loadPath() const;
    Q_INVOKABLE void loadVideo();
    Q_INVOKABLE void saveVideo();
    QString replayFps() const;
    bool canReplayStream() const;
    Q_INVOKABLE void startReplayStream();
    Q_INVOKABLE void stopReplayStream();
    bool isReplayStreaming() const;

    bool detectCheckerBoardEnabled() const;
    QString calibrationResultString() const;
    bool applyCalibrationEnabled() const;

    bool detectArucoEnabled() const;
    bool kallmanFilterEnabled() const;

    Video* video();
    int frameIndex() const;
    QString frameIndexString() const;
    QString frameFileName() const;
    Frame* frame() const;
    Q_INVOKABLE void frameFirst();
    Q_INVOKABLE void framePrevious();
    Q_INVOKABLE void frameNext();
    Q_INVOKABLE void deleteFrame();
    Q_INVOKABLE void calibrateUsingAll();
    Q_INVOKABLE void saveCalibration();
    Q_INVOKABLE void loadCalibration();
    Q_INVOKABLE void detectArucoInAll();

public slots:
    void setVideoDevice(QString videoDevice);
    void setConnectPossible(bool connectPossible);
    void setVideoFormats(QStringList videoFormats);
    void setCurrentVideoFormatIndex(int currentVideoFormatIndex);
    void setExposure(int exposure);
    void setCanCameraStream(bool canCameraStream);
    void setIsCameraStreaming(bool isCameraStreaming);
    void cameraImageReceived(QImage image);
    void setImage(QImage image);
    void setFrameIndex(int index);
    void setFps(qreal fps);
    void setSavePath(QString savePath);
    void setSaveallframesEnabled(bool saveallframesEnabled);
    void setSkipSavingFrames(int skipSavingFrames);
    void setLoadPath(QString loadPath);
    void setReplayFps(QString replayFps);
    void setCanReplayStream(bool canReplayStream);
    void setIsReplayStreaming(bool isReplayStreaming);
    void setDetectCheckerBoardEnabled(bool detectCheckerBoardEnabled);
    void setCalibrationResultString(QString calibrationResultString);
    void setApplyCalibrationEnabled(bool applyCalibrationEnabled);
    void setDetectArucoEnabled(bool detectArucoEnabled);
    void setKallmanFilterEnabled(bool kallmanFilterEnabled);
    void setFrameIndexString(QString frameIndexString);
    void setFrameFileName(QString frameFileName);

signals:
    void videoDeviceChanged(QString videoDevice);
    void connectPossibleChanged(bool connectPossible);
    void videoFormatsChanged(QStringList videoFormats);
    void currentVideoFormatIndexChanged(int currentVideoFormatIndex);
    void exposureChanged(int exposure);
    void canCameraStreamChanged(bool canCameraStream);
    void isCameraStreamingChanged(bool isCameraStreaming);
    void imageChanged(QImage image);
    void frameIndexChanged(int index);
    void fpsChanged(qreal fps);
    void savePathChanged(QString savePath);
    void saveallframesEnabledChanged(bool saveallframesEnabled);
    void skipSavingFramesChanged(int skipSavingFrames);
    void loadPathChanged(QString loadPath);
    void replayFpsChanged(QString replayFps);
    void canReplayStreamChanged(bool canReplayStream);
    void isReplayStreamingChanged(bool isReplayStreaming);
    void detectCheckerBoardEnabledChanged(bool detectCheckerBoardEnabled);
    void calibrationResultStringChanged(QString calibrationResultString);
    void applyCalibrationEnabledChanged(bool applyCalibrationEnabled);
    void detectArucoEnabledChanged(bool detectArucoEnabled);
    void kallmanFilterEnabledChanged(bool kallmanFilterEnabled);
    void frameIndexStringChanged(QString frameIndexString);
    void frameFileNameChanged(QString frameFileName);
    void frameChanged(Frame* frame);

private slots:
    void updateFps();
    void processImage();
    void updateFrameIndexString();

private:
    QString _videoDevice;
    bool _connectPossible;
    QScopedPointer<Camera> _camera;
    QStringList _videoFormats;
    int _currentVideoFormatIndex;
    int _exposure;
    bool _canCameraStream;
    bool _isCameraStreaming;
    QImage _image;
    bool _imageChanged;
    QImage _processedImage;
    qreal _fps;
    int _framesCounter;
    QElapsedTimer _elapsedTime;
    QString _savePath;
    bool _saveallframesEnabled;
    int _skipSavingFrames;
    int _skipSavingFramesCounter;
    ImageSaver _saver;
    QString _loadPath;
    QString _replayFps;
    bool _canReplayStream;
    ReplayTimer _replayTimer;
    bool _isReplayStreaming;
    bool _detectCheckerBoardEnabled;
    QScopedPointer<CameraCalibration> _cameraCalibration;
    QString _calibrationResultString;
    bool _applyCalibrationEnabled;
    QScopedPointer<ArucoDetector> _arucoDetector;
    bool _detectArucoEnabled;
    bool _kallmanFilterEnabled;
    Video _video;
    QString _frameIndexString;
    int _frameIndex;
    QString _frameFileName;
};

#endif // MAINCONTROLLER_H
