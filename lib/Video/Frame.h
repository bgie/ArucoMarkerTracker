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
#include <QDir>
#include <QFuture>
#include <QImage>
#include <QObject>

class Frame : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName CONSTANT)
    Q_PROPERTY(QImage image READ image CONSTANT)

public:
    explicit Frame(QDir path, QString fileName, QObject* parent = nullptr);
    explicit Frame(QImage image, QObject* parent = nullptr);
    virtual ~Frame() override;

    QString fileName() const;
    QImage image();

public slots:
    void loadImageFromDisk();

private:
    const QDir _path;
    const QString _fileName;
    QImage _image;
    bool _needsLoadFromDisk;
    QFuture<QImage> _imageFuture;
};
