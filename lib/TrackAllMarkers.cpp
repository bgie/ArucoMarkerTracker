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
#include "TrackAllMarkers.h"
#include "Frame.h"
#include "Marker.h"
#include <algorithm>
#include <math.h>

void trackAllMarkers(QList<Frame*> frames, double msecsPerFrame, const KalmanTracker3D::Params& p)
{
    QList<int> allIds = getAllMarkerIds(frames, false);

    // create a tracker and result vector for every id
    QHash<int, KalmanTracker3D*> trackers;
    for (auto id : allIds) {
        trackers.insert(id, new KalmanTracker3D(p));
    }

    // track every marker id in time
    const int frameCount = frames.size();
    for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
        Frame* frame = frames.at(frameIndex);
        QList<Marker*> filteredMarkers;

        // update markers that are found
        QSet<int> foundIds;
        for (Marker* marker : frame->markers()) {
            int id = marker->id();
            foundIds.insert(id);

            KalmanTracker3D* tracker = trackers.value(id);
            tracker->predict(msecsPerFrame);
            tracker->update(marker->position(), marker->rotation());

            filteredMarkers << new Marker(id, tracker->position(), tracker->rotation());
        }

        // predict markers that are not found
        for (int id : allIds) {
            if (!foundIds.contains(id)) {
                KalmanTracker3D* tracker = trackers.value(id);
                tracker->predict(msecsPerFrame);

                if (tracker->hasPosition()) {
                    filteredMarkers << new Marker(id, tracker->position(), tracker->rotation());
                }
            }
        }

        frame->setFilteredMarkers(filteredMarkers);
    }
}

void writeAllMarkersToCsv(QList<Frame*> frames, QString writeCsvFilename, bool details)
{
    QFile file(writeCsvFilename);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);

        QList<int> allIds = getAllMarkerIds(frames, false);

        for (int id : allIds) {
            stream << "id," << id << "\n";
            stream << "frame,x_measured,x_filtered,dist,y_measured,y_filtered,dist,z_measured,z_filtered,dist,x_rotation,y_rotation,z_rotation\n";

            QVector3D measuredSum, filteredSum;
            QVector<QVector3D> allMeasured, allFiltered;

            for (int frameIndex = 0; frameIndex < frames.size(); ++frameIndex) {
                Frame* frame = frames.at(frameIndex);
                QVector3D measured, filtered, rotation;
                auto markerIt = std::find_if(
                    frame->markers().constBegin(), frame->markers().constEnd(),
                    [id](Marker* m) { return m->id() == id; });
                bool hasMeasurement = markerIt != frame->markers().constEnd();
                if (hasMeasurement) {
                    measured = (*markerIt)->position();
                    rotation = (*markerIt)->rotation();
                    measuredSum += measured;
                    allMeasured << measured;
                }
                markerIt = std::find_if(
                    frame->filteredMarkers().constBegin(), frame->filteredMarkers().constEnd(),
                    [id](Marker* m) { return m->id() == id; });
                bool hasFiltered = markerIt != frame->filteredMarkers().constEnd();
                if (hasFiltered) {
                    filtered = (*markerIt)->position();
                    filteredSum += filtered;
                    allFiltered << filtered;
                }

                if (details) {
                    stream << frameIndex;
                    stream << ",";
                    if (hasMeasurement) {
                        stream << measured.x();
                    }
                    stream << ",";
                    if (hasFiltered) {
                        stream << filtered.x();
                    }
                    stream << ",";
                    if (hasMeasurement && hasFiltered) {
                        stream << filtered.x() - measured.x();
                    }
                    stream << ",";
                    if (hasMeasurement) {
                        stream << measured.y();
                    }
                    stream << ",";
                    if (hasFiltered) {
                        stream << filtered.y();
                    }
                    stream << ",";
                    if (hasMeasurement && hasFiltered) {
                        stream << filtered.y() - measured.y();
                    }
                    stream << ",";
                    if (hasMeasurement) {
                        stream << measured.z();
                    }
                    stream << ",";
                    if (hasFiltered) {
                        stream << filtered.z();
                    }
                    stream << ",";
                    if (hasMeasurement && hasFiltered) {
                        stream << filtered.z() - measured.z();
                    }
                    stream << ",";
                    if (hasMeasurement) {
                        stream << rotation.x();
                    }
                    stream << ",";
                    if (hasMeasurement) {
                        stream << rotation.y();
                    }
                    stream << ",";
                    if (hasMeasurement) {
                        stream << rotation.z();
                    }
                    stream << "\n";
                }
            }

            QVector3D measuredAvg, filteredAvg, measuredStdev, filteredStdev;
            bool hasMeasurement = allMeasured.size();
            if (hasMeasurement) {
                measuredAvg = measuredSum / allMeasured.size();
                measuredStdev = std::accumulate(allMeasured.constBegin(), allMeasured.constEnd(), QVector3D(), [measuredAvg](QVector3D acc, QVector3D val) -> QVector3D {
                    QVector3D delta = measuredAvg - val;
                    return acc + (delta * delta);
                }) / allMeasured.size();
            }
            bool hasFiltered = allFiltered.size();
            if (hasFiltered) {
                filteredAvg = filteredSum / allFiltered.size();
                filteredStdev = std::accumulate(allMeasured.constBegin(), allMeasured.constEnd(), QVector3D(), [filteredAvg](QVector3D acc, QVector3D val) -> QVector3D {
                    QVector3D delta = filteredAvg - val;
                    return acc + (delta * delta);
                }) / allFiltered.size();
            }

            stream << "STDEV,";
            if (hasMeasurement) {
                stream << sqrt(measuredStdev.x());
            }
            stream << ",";
            if (hasFiltered) {
                stream << sqrt(filteredStdev.x());
            }
            stream << ",,";
            if (hasMeasurement) {
                stream << sqrt(measuredStdev.y());
            }
            stream << ",";
            if (hasFiltered) {
                stream << sqrt(filteredStdev.y());
            }
            stream << ",,";
            if (hasMeasurement) {
                stream << sqrt(measuredStdev.z());
            }
            stream << ",";
            if (hasFiltered) {
                stream << sqrt(filteredStdev.z());
            }
            stream << ",\n\n";
        }
    }
}

QList<int> getAllMarkerIds(QList<Frame*> frames, bool includeFiltered)
{
    QSet<int> result;
    for (Frame* frame : frames) {
        for (Marker* marker : frame->markers()) {
            result.insert(marker->id());
        }
        if (includeFiltered) {
            for (Marker* marker : frame->filteredMarkers()) {
                result.insert(marker->id());
            }
        }
    }
    QList<int> list = result.toList();
    std::sort(list.begin(), list.end());
    return list;
}
