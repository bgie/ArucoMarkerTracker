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
#include "ImageSaver.h"
#include <QImage>
#include <QObject>

class RecordController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString savePath READ savePath WRITE setSavePath NOTIFY savePathChanged)
    Q_PROPERTY(bool saveallframesEnabled READ saveallframesEnabled WRITE setSaveallframesEnabled NOTIFY saveallframesEnabledChanged)
    Q_PROPERTY(int skipSavingFrames READ skipSavingFrames WRITE setSkipSavingFrames NOTIFY skipSavingFramesChanged)

public:
    explicit RecordController(QObject* parent = nullptr);
    virtual ~RecordController();

    QString savePath() const;
    bool saveallframesEnabled() const;
    int skipSavingFrames() const;
    Q_INVOKABLE void saveSingleFrame();

public slots:
    void setImage(QImage image);
    void setSavePath(QString savePath);
    void setSaveallframesEnabled(bool saveallframesEnabled);
    void setSkipSavingFrames(int skipSavingFrames);

signals:
    void savePathChanged(QString savePath);
    void saveallframesEnabledChanged(bool saveallframesEnabled);
    void skipSavingFramesChanged(int skipSavingFrames);

private:
    QString _savePath;
    bool _saveallframesEnabled;
    int _skipSavingFrames;
    int _skipSavingFramesCounter;
    ImageSaver _saver;
    QImage _image;
};
