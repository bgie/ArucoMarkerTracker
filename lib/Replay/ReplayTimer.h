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
#ifndef IMAGESREPLAYER_H
#define IMAGESREPLAYER_H

#include <QElapsedTimer>
#include <QObject>
#include <QTimer>

class ReplayTimer : public QObject {
    Q_OBJECT

public:
    explicit ReplayTimer(QObject* parent = nullptr);

    void setFrameCount(int count);
    void setFps(qreal fps);

    bool canReplay() const;
    void setAutoPlayEnabled(bool enabled);

signals:
    void playFrame(int index);

private slots:
    void timeout();

private:
    QTimer _timer;
    QElapsedTimer _elapsedTime;
    int _frameCount;
    qreal _fps;
    bool _autoPlay;
    int _sentFrames;
    int _lastFrameIndex;
};

#endif // IMAGESREPLAYER_H
