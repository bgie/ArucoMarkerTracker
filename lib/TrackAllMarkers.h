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
#ifndef TRACKALLMARKERS_H
#define TRACKALLMARKERS_H

#include "MarkerTracker.h"
#include <QList>

class Frame;

QList<int> getAllMarkerIds(QList<Frame*> frames, bool includeFiltered = true);
void trackAllMarkers(QList<Frame*> frames, double msecsPerFrame, const MarkerTracker::Params& p);
void writeAllMarkersToCsv(QList<Frame*> frames, QString writeCsvFilename, bool details = false);

#endif // TRACKALLMARKERS_H
