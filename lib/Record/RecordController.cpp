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
#include "RecordController.h"
#include <QDir>
#include <QSettings>
#include <Video/VideoSource.h>

namespace {
const QString SAVEPATH_KEY(QStringLiteral("SavePath"));
const QString SKIPSAVINGFRAMES_KEY(QStringLiteral("SkipSavingFrames"));
}

RecordController::RecordController(QObject* parent)
    : QObject(parent)
    , _saveallframesEnabled(false)
    , _skipSavingFrames(0)
    , _skipSavingFramesCounter(0)
    , _videoSource(nullptr)
{
    QSettings settings;
    setSavePath(settings.value(SAVEPATH_KEY, QDir::homePath()).toString());
    setSkipSavingFrames(settings.value(SKIPSAVINGFRAMES_KEY, 0).toInt());
}

RecordController::~RecordController()
{
}

void RecordController::setSavePath(QString savePath)
{
    if (_savePath == savePath)
        return;

    _savePath = savePath;

    QSettings settings;
    settings.setValue(SAVEPATH_KEY, _savePath);

    _saver.setPath(_savePath);

    emit savePathChanged(_savePath);
}

void RecordController::setSaveallframesEnabled(bool saveallframesEnabled)
{
    if (_saveallframesEnabled == saveallframesEnabled)
        return;

    _saveallframesEnabled = saveallframesEnabled;

    _saver.setEnabled(_saveallframesEnabled);

    emit saveallframesEnabledChanged(_saveallframesEnabled);
}

void RecordController::setSkipSavingFrames(int skipSavingFrames)
{
    if (_skipSavingFrames == skipSavingFrames)
        return;

    _skipSavingFrames = skipSavingFrames;
    _skipSavingFramesCounter = _skipSavingFrames;

    QSettings settings;
    settings.setValue(SKIPSAVINGFRAMES_KEY, _skipSavingFrames);

    emit skipSavingFramesChanged(_skipSavingFrames);
}

void RecordController::setVideoSource(VideoSource* videoSource)
{
    if (_videoSource == videoSource)
        return;

    if (_videoSource) {
        disconnect(_videoSource, 0, this, 0);
    }

    _videoSource = videoSource;

    if (_videoSource) {
        connect(_videoSource, &VideoSource::frameChanged, this, &RecordController::setFrame);
    }
    emit videoSourceChanged(_videoSource);
}

void RecordController::setFrame(Frame* f)
{
    if (_frame == f)
        return;

    _frame = f;

    if (_saveallframesEnabled && _frame && _frame->fileName().isEmpty()) {
        if (_skipSavingFramesCounter == 0) {
            _saver.saveImage(_frame->image());

            _skipSavingFramesCounter = _skipSavingFrames;
        } else {
            _skipSavingFramesCounter--;
        }
    }
}

QString RecordController::savePath() const
{
    return _savePath;
}

bool RecordController::saveallframesEnabled() const
{
    return _saveallframesEnabled;
}

void RecordController::saveSingleFrame()
{
    if (!_saveallframesEnabled && _frame && _frame->fileName().isEmpty()) {
        _saver.saveSingleImage(_frame->image());
    }
}

VideoSource* RecordController::videoSource() const
{
    return _videoSource;
}

int RecordController::skipSavingFrames() const
{
    return _skipSavingFrames;
}
