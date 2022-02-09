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
#include "ObjectTracker.h"
#include "Marker.h"
#include <QDebug>
#include <QImage>
#include <QMutexLocker>

ObjectTracker::ObjectTracker(Aruco* aruco, QObject* parent)
    : QObject(parent)
    , _aruco(aruco)
    , _framesPerSecond(30)
{
}

ObjectTracker::~ObjectTracker()
{
    qDeleteAll(_idToMarker.values());
}

void ObjectTracker::processFrame(QImage image)
{
    if (_aruco) {
        auto markers = _aruco->detectMarkers(image);
        auto angles = _aruco->calc2dAngles(markers);

        {
            QMutexLocker lock(&_mutex);
            const float msecsPerFrame = 1000 / _framesPerSecond;
            QSet<int> foundIds;
            bool newIdAdded = false;
            for (size_t i = 0; i < markers.ids.size(); ++i) {
                const int id = markers.ids.at(i);
                foundIds << id;

                if (!_idToMarker.contains(id)) {
                    newIdAdded = true;
                    _idToMarker[id] = new Marker(id);
                }
                auto tvec = markers.tvecs.at(i);
                auto rvec = markers.rvecs.at(i);
                float angle = angles.at(i);
                _idToMarker[id]->setPositionRotation(QVector3D(tvec[0], tvec[1], tvec[2]), angle, msecsPerFrame);
            }
            auto missingIds = _idToMarker.keys().toSet() - foundIds;
            for (auto id : missingIds) {
                _idToMarker[id]->setNotDetected(msecsPerFrame);
            }

            _image = image;
            _markers = markers;
        }

        emit imageChanged(image);
    }
}

QMutex* ObjectTracker::mutex()
{
    return &_mutex;
}

QImage ObjectTracker::image() const
{
    return _image;
}

const Aruco::Markers& ObjectTracker::markers() const
{
    return _markers;
}

QMap<int, Marker*> ObjectTracker::idToMarker() const
{
    return _idToMarker;
}

float ObjectTracker::framesPerSecond() const
{
    return _framesPerSecond;
}

void ObjectTracker::setFramesPerSecond(float framesPerSecond)
{
    if (qFuzzyCompare(_framesPerSecond, framesPerSecond))
        return;

    _framesPerSecond = framesPerSecond;
    emit framesPerSecondChanged(_framesPerSecond);
}
