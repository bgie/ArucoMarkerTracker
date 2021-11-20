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
#include "GeneticAlgorithm.h"
#include "MarkerTracker.h"
#include "Video.h"
#include <QCoreApplication>
#include <QSet>
#include <printf.h>

const double FRAMERATE_MSEC(1000.0f / 30.0f);

class MarkerTrackerEvaluator : public IEvaluator {
public:
    MarkerTrackerEvaluator(const Video& video)
        : _video(video)
        , _minId(-1)
        , _maxId(-1)
    {
        // determine which ids exist
        for (Frame* frame : video.frames()) {
            for (Marker* marker : frame->markers()) {
                int id = marker->id();
                _allIds.insert(id);

                if (_minId == -1) {
                    _minId = _maxId = id;
                } else if (_minId > id) {
                    _minId = id;
                } else if (_maxId < id) {
                    _maxId = id;
                }
            }
        }
    }

    virtual ~MarkerTrackerEvaluator() override { }

    virtual float evaluate(Genome genome) const override
    {
        auto log = evaluateImpl(genome);
        double error = 0.;

        for (auto it = log.constBegin(); it != log.constEnd(); ++it) {
            if (it.key() == 45) {
                QVector3D sum;
                int count = 0;
                for (const auto& entry : it.value()) {
                    if (entry.hasMarker) {
                        sum += entry.updatePosition;
                        count++;
                    }
                }
                QVector3D avg = sum / count;
                QVector3D errsum;
                for (const auto& entry : it.value()) {
                    if (entry.hasMarker) {
                        QVector3D err = entry.updatePosition - avg;
                        errsum += err * err;
                    }
                }
                error = errsum.x() + errsum.y() + errsum.z();
            }
        }
        return error;
    }

    struct TrackingEntry {
        bool hasMarker;
        QVector3D markerPosition;
        QVector3D updatePosition;

        bool isPredicted;
        QVector3D predictPosition;
    };

    void writeVectorIfExists(QTextStream& stream, QVector3D vec, bool exists)
    {
        stream << ",";
        if (exists)
            stream << vec.x();
        stream << ",";
        if (exists)
            stream << vec.y();
        stream << ",";
        if (exists)
            stream << vec.z();
    }

    void writeCsv(Genome genome, QString writeCsvFilename)
    {
        auto log = evaluateImpl(genome);

        QFile file(writeCsvFilename);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);

            for (int id = _minId; id < _maxId; ++id)
                if (_allIds.contains(id)) {
                    const auto& entries = log.value(id);
                    stream << "id," << id << "\n";
                    stream << "frame,marker_x,marker_y,marker_z,predict_x,predict_y,predict_z,update_x,update_y,update_z,predict_dist,update_dist\n";

                    for (int i = 0; i < entries.size(); ++i) {
                        const auto& entry = entries.at(i);
                        stream << i;
                        writeVectorIfExists(stream, entry.markerPosition, entry.hasMarker);
                        writeVectorIfExists(stream, entry.predictPosition, entry.isPredicted);
                        writeVectorIfExists(stream, entry.updatePosition, entry.hasMarker);
                        stream << "," << entry.markerPosition.distanceToPoint(entry.predictPosition);
                        stream << "," << entry.markerPosition.distanceToPoint(entry.updatePosition);
                        stream << "\n";
                    }
                    stream << "\n";
                }
        }
    }

    QHash<int, QVector<TrackingEntry>> evaluateImpl(Genome genome) const
    {
        QHash<int, QVector<TrackingEntry>> result;

        MarkerTracker::Params p;
        p.positionXYProcessNoiseCov = genome.at(0);
        p.positionZProcessNoiseCov = genome.at(1);
        p.velocityXYProcessNoiseCov = genome.at(2);
        p.velocityZProcessNoiseCov = genome.at(3);
        p.measurementXYNoiseCov = genome.at(4);
        p.measurementZNoiseCov = genome.at(5);

        // create a tracker and result vector for every id
        QHash<int, MarkerTracker*> trackers;        
        for (auto id : _allIds) {
            trackers.insert(id, new MarkerTracker(p));
            result.insert(id, QVector<TrackingEntry>());
        }

        // track every marker id in time
        const int frameCount = _video.frames().size();
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            Frame* frame = _video.frames().at(frameIndex);

            // update markers that are found
            QSet<int> ids;
            for (Marker* marker : frame->markers()) {
                int id = marker->id();
                ids.insert(id);

                TrackingEntry entry;
                entry.hasMarker = true;
                entry.markerPosition = marker->position();

                MarkerTracker* tracker = trackers.value(id);
                tracker->predict(FRAMERATE_MSEC);
                entry.isPredicted = tracker->hasPosition();
                entry.predictPosition = tracker->position();

                tracker->update(marker->position(), marker->rotation());
                entry.updatePosition = tracker->position();

                result[id].append(entry);
            }

            // predict markers that are not found
            for (int id : _allIds - ids) {
                MarkerTracker* tracker = trackers.value(id);
                tracker->predict(FRAMERATE_MSEC);
                tracker->updateNotFound();

                TrackingEntry entry;
                entry.hasMarker = false;
                entry.isPredicted = tracker->hasPosition();
                entry.predictPosition = tracker->position();
                result[id].append(entry);
            }
        }
        qDeleteAll(trackers.values());
        return result;
    }

private:
    const Video& _video;
    QSet<int> _allIds;
    int _minId, _maxId;
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    Video video;
    video.load(QStringLiteral(":/"), QStringLiteral("TanksVideo.json"));

    const int genomeCount = 12;
    const float minGeneInit = 1e-7f;
    const float maxGeneInit = 1e2;
    const float minGeneValue = 1e-7f;
    const float maxGeneValue = 1e7f;
    const float mutationMulProbability = 0.1f;
    const float mutationMulStdDev = 0.8f;
    const float mutationAddProbability = 0.1f;
    const float mutationAddStdDev = 1.0f;
    float crossOverMinAmount = -0.2f;
    float crossOverMaxAmount = 0.4f;
    GenomeLab lab(genomeCount, minGeneInit, maxGeneInit, minGeneValue, maxGeneValue, mutationMulProbability, mutationMulStdDev, mutationAddProbability, mutationAddStdDev, crossOverMinAmount, crossOverMaxAmount);
    MarkerTrackerEvaluator evaluator(video);

    const int populationSize = 200;
    const float elitePortion = 0.02f;
    float rankSelectionMultiplier = 16.0f;
    GeneticAlgorithm ga(lab, evaluator, populationSize, elitePortion, rankSelectionMultiplier);

    ga.seed(QList<Genome>() << Genome({ 1, 1.5, 393216, 8192, 8, 2, 1.5, 1.5, 6144, 256, 16, 0 }));

    for (int generation = 0; generation < 200; ++generation) {
        float lowestError = ga.evaluateAll();
        qDebug() << "Gen:" << generation << ", Error:" << lowestError << ", Genome:" << ga.genomes().first();

        evaluator.writeCsv(ga.genomes().first(), QStringLiteral("gen%1.csv").arg(generation, 3, 10, QChar('0')));

        // RESULTS:
        // simple model from tutorial, 10 parameters
        //  Gen: 243 , Error: 1.39904e+07 , Genome: QVector(3371.03, 8.06407, 8021.29, 556450, 0.00337474, 3.37883e-05, 0.0833277, 0.00880451, 1235.23, 1186.9)

        // Added position-velocity terms to process noise covariance (Q)
        // Gen: 203 , Error: 1.20778e+07 , Genome: QVector(7.98289, 2.9827, 15918.3, 53356.6, 0.0620618, 1.58544, 0.0659963, 838.297, 5430.78, 1015.85, 123355, 1178.54)

        // only consider error after update
        // Gen: 199 , Error: 1.92069 , Genome: QVector(0.0221421, 0.00619257, 39686.6, 771470, 0.038884, 0.00304861, 0.0248161, 0.100201, 441.631, 289.662, 1.63181, 31659.3)

        // only consider error after predict
        // Gen: 199 , Error: 6.23437e+06 , Genome: QVector(11521.6, 0.0470789, 0.0270152, 1.49219e-11, 0.000545563, 0.0449313, 0.295824, 0.0496183, 0.0109412, 1.49363e-07, 0.945787, 7.45083e-06)

        ga.nextGeneration();
    }
}
