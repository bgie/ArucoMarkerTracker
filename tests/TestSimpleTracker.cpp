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
#include "TestSimpleTracker.h"
#include "GeneticAlgorithm.h"
#include "SimpleTracker.h"
#include "TestFactory.h"
#include <math.h>

REGISTER_TESTCLASS(TestSimpleTracker);

class SimpleTrackerEvaluator : public IEvaluator {
public:
    SimpleTrackerEvaluator()
    {
        _measuredPos = QVector<double>(200);
        _actualPos = QVector<double>(200);

        double actualPos = 1000;
        const int frameToStartMoving = 555;
        const double velocity = 5;
        for (int i = 0; i < _measuredPos.size(); ++i) {
            if (i >= frameToStartMoving) {
                actualPos += velocity;
            }
            _actualPos[i] = actualPos;
            _measuredPos[i] = randomGaussianFloat(actualPos, 4);
        }
    }
    virtual ~SimpleTrackerEvaluator() override { }

    virtual float evaluate(Genome genome) const override
    {
        const int loops = 10;
        double result = 0;
        for (int i = 0; i < loops; ++i) {
            result += evaluateImpl(genome, false);
        }
        return result / loops;
    }

    float evaluateImpl(Genome genome, bool debugPrint) const
    {
        SimpleTracker::Params p;
        p.positionProcessNoiseCov = genome.at(0);
        p.velocityProcessNoiseCov = genome.at(1);
        p.measurementNoiseCov = genome.at(2);
        SimpleTracker tracker(p);

        double totalError = 0.0;

        const double timeStep = 1;
        for (int i = 0; i < _measuredPos.size(); ++i) {
            double actualPos = _actualPos.at(i);
            double measuredPos = randomGaussianFloat(actualPos, 1); //_measuredPos.at(i);

            tracker.predict(timeStep);
            double predictedPos = tracker.position();

            tracker.update(measuredPos);
            double improvedPos = tracker.position();

            double residualError = (improvedPos - actualPos);
            totalError += qAbs(residualError); // * residualError;

            if (debugPrint) {
                qDebug() << actualPos << measuredPos << predictedPos << improvedPos << "\tactual diff:" << residualError << "\tmeas diff:" << improvedPos - measuredPos << "\tspeed:" << tracker.velocity();
            }

            // start moving from frame 50 using speed 20, measurement noise 1 stdev
            // QVector(0.0297066, 301.268, 84.9195)
            //
            // never move - measurement noise 1 stdev
            // QVector(1e-10, 1e-10, 5.55003)
            //
            // always move using speed 100 - measurement noise 1 stdev
            // Error: 0.383901 , Genome: QVector(1e-10, 1e-10, 0.0734406)
            // Error: 0.254322 , Genome: QVector(1.83937e-10, 1e-10, 0.00573337)
            // Error: 0.256307 , Genome: QVector(1e-10, 1e-10, 0.00246078)
            //
            // never move
            // Error: 0.207173 , Genome: QVector(1e-10, 1e-10, 4.4137)
            //
            // move from frame 50 using speed 100
            // Error: 0.951037 , Genome: QVector(25.6948, 64.1084, 1.01879)
            //
            // SAME - but using avg error instead of stdev
            // Error: 0.748828 , Genome: QVector(9.8889, 33.0928, 2.97065)
            // Error: 0.739686 , Genome: QVector(18.7133, 384.486, 3.06905)
            //
            // move from frame 50 using speed 5
            // Error: 0.538095 , Genome: QVector(7.44314, 12.1084, 312.402)
        }
        return /*sqrt*/ (totalError / _measuredPos.size());
    }

private:
    QVector<double> _actualPos;
    QVector<double> _measuredPos;
};

void TestSimpleTracker::simple_tracker_must_track_stationary_object()
{
    const int genomeCount = 3;
    const float minGeneInit = 1e-7f;
    const float maxGeneInit = 1e2;
    const float minGeneValue = 1e-10f;
    const float maxGeneValue = 1e10f;
    const float mutationMulProbability = 0.2f;
    const float mutationMulStdDev = 2.0f;
    const float mutationAddProbability = 0.1f;
    const float mutationAddStdDev = 1.0f;
    float crossOverMinAmount = -0.2f;
    float crossOverMaxAmount = 0.4f;
    GenomeLab lab(genomeCount, minGeneInit, maxGeneInit, minGeneValue, maxGeneValue, mutationMulProbability, mutationMulStdDev, mutationAddProbability, mutationAddStdDev, crossOverMinAmount, crossOverMaxAmount);
    SimpleTrackerEvaluator evaluator;

    const int populationSize = 1000;
    const float elitePortion = 0.02f;
    float rankSelectionMultiplier = 100.0f;
    GeneticAlgorithm ga(lab, evaluator, populationSize, elitePortion, rankSelectionMultiplier);

    Genome currentBest = Genome({ 1e-10, 1e-10, 0.002 });
    ga.seed(QList<Genome>({ currentBest }));

    for (int generation = 0; generation < 10; ++generation) {
        float lowestError = ga.evaluateAll();
        qDebug() << "Gen:" << generation << ", Error:" << lowestError << ", Genome:" << ga.genomes().first();

        ga.nextGeneration();
    }
    evaluator.evaluateImpl(ga.genomes().first(), true);
    evaluator.evaluateImpl(currentBest, true);
}
