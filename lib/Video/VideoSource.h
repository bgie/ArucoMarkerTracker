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
#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include "Frame.h"
#include <QObject>
#include <functional>

class VideoSource : public QObject {
    Q_OBJECT
    Q_PROPERTY(Frame* frame READ frame WRITE setFrame NOTIFY frameChanged)

public:
    explicit VideoSource(QObject* parent = nullptr);
    virtual ~VideoSource() override;

    Frame* frame() const;
    void setFrame(Frame* f, bool takesOwnership = false);

    void setExclusiveFrameProvider(std::function<void()> endedCallback);

signals:
    void sourceObjectChanged(QObject* source);
    void frameChanged(Frame* f);

private:
    Frame* _frame;
    bool _hasOwnership;
    std::function<void()> _exclusiveProviderEndedCallback;
};

#endif // VIDEOSOURCE_H
