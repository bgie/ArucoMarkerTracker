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
#include <QElapsedTimer>
#include <QImage>
#include <QObject>
#include <QScopedPointer>

class Camera : public QObject {
    Q_OBJECT

public:
    explicit Camera(QString deviceName, QObject *parent = nullptr);
    virtual ~Camera();

    QString deviceName() const;
   
    QStringList videoFormats() const;
    void setVideoFormatIndex(int i);

    void setExposure(int val);
    void setGain(int val);

    bool canStream() const;

    void startStream();
    void stopStream();

    static bool isValidDevice(QString deviceName);

signals:
    void frameRead(const QImage image, QElapsedTimer timer);

private:
    void updateExposure();
    void updateGain();

private:
    struct Data;
    QScopedPointer<Data> _d;
};
