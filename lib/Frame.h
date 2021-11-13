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
#ifndef FRAME_H
#define FRAME_H

#include "Marker.h"
#include <QFuture>
#include <QImage>
#include <QObject>

class Frame : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName CONSTANT)
    Q_PROPERTY(QString chessBoardReprojectionError READ chessBoardReprojectionError WRITE setChessBoardReprojectionError NOTIFY chessBoardReprojectionErrorChanged)
    Q_PROPERTY(QList<QObject*> markers READ markersQObjects NOTIFY markersChanged)

public:
    explicit Frame(QString fileName, QString fullFileName, QObject* parent = nullptr);
    virtual ~Frame() override;

    QString fileName() const;
    QImage image() const;

    QString chessBoardReprojectionError() const;

    QList<QObject*> markersQObjects() const;
    QList<Marker*> markers() const;

public slots:
    void setChessBoardReprojectionError(QString chessBoardReprojectionError);
    void setMarkers(QList<Marker*> list);

signals:
    void chessBoardReprojectionErrorChanged(QString chessBoardReprojectionError);
    void markersChanged();

private:
    const QString _fileName;
    QFuture<QImage> _image;
    QString _chessBoardReprojectionError;
    QList<Marker*> _markers;
};

#endif // FRAME_H
