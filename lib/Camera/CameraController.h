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
#pragma once
#include <QImage>
#include <QObject>

class Camera;

class CameraController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString videoDevice READ videoDevice WRITE setVideoDevice NOTIFY videoDeviceChanged)
    Q_PROPERTY(bool connectPossible READ connectPossible NOTIFY connectPossibleChanged)
    Q_PROPERTY(QStringList videoFormats READ videoFormats NOTIFY videoFormatsChanged)
    Q_PROPERTY(int currentVideoFormatIndex READ currentVideoFormatIndex WRITE setCurrentVideoFormatIndex NOTIFY currentVideoFormatIndexChanged)
    Q_PROPERTY(int exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(int gain READ gain WRITE setGain NOTIFY gainChanged)
    Q_PROPERTY(bool canCameraStream READ canCameraStream NOTIFY canCameraStreamChanged)
    Q_PROPERTY(bool isCameraStreaming READ isCameraStreaming NOTIFY isCameraStreamingChanged)

public:
    explicit CameraController(QObject* parent = nullptr);
    virtual ~CameraController();

    QString videoDevice() const;
    bool connectPossible() const;
    QStringList videoFormats() const;
    int currentVideoFormatIndex() const;
    int exposure() const;
    int gain() const;
    Q_INVOKABLE void connect();
    bool canCameraStream() const;
    Q_INVOKABLE void startCameraStream();
    Q_INVOKABLE void stopCameraStream();
    bool isCameraStreaming() const;

public slots:
    void setVideoDevice(QString videoDevice);
    void setVideoFormats(QStringList videoFormats);
    void setCurrentVideoFormatIndex(int currentVideoFormatIndex);
    void setExposure(int value);
    void setGain(int value);

signals:
    void videoDeviceChanged(QString videoDevice);
    void connectPossibleChanged(bool connectPossible);
    void videoFormatsChanged(QStringList videoFormats);
    void currentVideoFormatIndexChanged(int currentVideoFormatIndex);
    void exposureChanged(int exposure);
    void gainChanged(int value);
    void canCameraStreamChanged(bool canCameraStream);
    void isCameraStreamingChanged(bool isCameraStreaming);
    void imageChanged(QImage image);

private slots:
    void setConnectPossible(bool connectPossible);
    void setCanCameraStream(bool canCameraStream);
    void setIsCameraStreaming(bool isCameraStreaming);

private:
    QString _videoDevice;
    bool _connectPossible;
    QScopedPointer<Camera> _camera;
    QStringList _videoFormats;
    int _currentVideoFormatIndex;
    int _exposure;
    int _gain;
    bool _canCameraStream;
    bool _isCameraStreaming;
};
