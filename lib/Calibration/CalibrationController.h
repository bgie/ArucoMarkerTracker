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

class Aruco;
class Video;
class FramesCalibrationModel;
class CameraCalibration;

class CalibrationController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Aruco* aruco READ aruco WRITE setAruco NOTIFY arucoChanged)
    Q_PROPERTY(QString loadPath READ loadPath WRITE setLoadPath NOTIFY loadPathChanged)
    Q_PROPERTY(QString saveFilename READ saveFilename WRITE setSaveFilename NOTIFY saveFilenameChanged)
    Q_PROPERTY(int frameIndex READ frameIndex WRITE setFrameIndex NOTIFY frameIndexChanged)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(FramesCalibrationModel* model READ model CONSTANT)
    Q_PROPERTY(QString totalError READ totalError NOTIFY totalErrorChanged)
    Q_PROPERTY(QString calibrationValues READ calibrationValues NOTIFY calibrationValuesChanged)

public:
    explicit CalibrationController(QObject* parent = nullptr);
    virtual ~CalibrationController() override;

    Aruco* aruco() const;

    QString loadPath() const;
    QString saveFilename() const;

    QImage image() const;
    int frameIndex() const;

    FramesCalibrationModel* model() const;

    Q_INVOKABLE void calibrate();
    QString calibrationValues() const;
    QString totalError() const;
    Q_INVOKABLE void saveCalibration();
    Q_INVOKABLE void loadCalibration();

public slots:
    void setAruco(Aruco* aruco);
    void setLoadPath(QString loadPath);
    void setSaveFilename(QString saveFilename);
    void setFrameIndex(int index);
    void setTotalError(QString totalError);
    void setCalibrationValues(QString calibrationValues);

signals:
    void arucoChanged(Aruco* aruco);
    void loadPathChanged(QString loadPath);
    void saveFilenameChanged(QString saveFilename);
    void imageChanged(QImage image);
    void frameIndexChanged(int index);
    void totalErrorChanged(QString totalError);
    void calibrationValuesChanged(QString calibrationValues);

private:
    QString _loadPath;
    QString _saveFilename;
    Video* _video;
    QImage _image;
    int _frameIndex;
    FramesCalibrationModel* _model;
    CameraCalibration* _calibration;
    QString _totalError;
    QString _calibrationValues;
    Aruco* _aruco;
};
