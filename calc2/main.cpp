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
#include "Video.h"
#include <QCoreApplication>

const double MSECS_PER_FRAME = 1000.0 / 30.0;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    Video video;
    video.load(QStringLiteral(":/"), QStringLiteral("Tanks2Video.json"));

    MarkerTracker::Params p;
    trackAllMarkers(video.frames(), MSECS_PER_FRAME, p);
    writeAllMarkersToCsv(video.frames(), "markers.csv", true);

    double grid[] = { 1e-8, 1e0 };

    for (double noiseM : grid) {
        for (double noisePP : grid) {
            for (double noisePV : grid) {
                p.measurementXYNoiseCov = p.measurementZNoiseCov = noiseM;
                p.positionXYProcessNoiseCov = p.positionZProcessNoiseCov = noisePP;
                p.velocityXYProcessNoiseCov = p.velocityZProcessNoiseCov = noisePV;
                trackAllMarkers(video.frames(), MSECS_PER_FRAME, p);
                writeAllMarkersToCsv(video.frames(), QStringLiteral("markers%1-%2-%3.csv").arg(noiseM).arg(noisePP).arg(noisePV), true);
            }
        }
    }
}
