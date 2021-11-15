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
#include "TestGeneraticAlgorithm.h"
#include "GeneticAlgorithm.h"
#include "TestFactory.h"
#include <QDebug>
#include <math.h>

REGISTER_TESTCLASS(TestGeneraticAlgorithm);

void TestGeneraticAlgorithm::lab_biogenesis_must_create_valid_genome()
{
    const int genomeCount = 4;
    const float minGeneValue = -1.0f;
    const float maxGeneValue = 1.0f;
    GenomeLab lab(genomeCount, minGeneValue, maxGeneValue, minGeneValue, maxGeneValue, 0, 0, 0, 0);

    auto result = lab.biogenesis();
    QCOMPARE(result.size(), genomeCount);
    for (int i = 0; i < genomeCount; ++i) {
        QVERIFY(result.at(i) >= minGeneValue);
        QVERIFY(result.at(i) <= maxGeneValue);
    }
}

void TestGeneraticAlgorithm::lab_mutate_must_mutate()
{
    const int genomeCount = 1;
    const float mutationProbability = 1.0f;
    const float mutationStrengthStdDev = 1e9;
    GenomeLab lab(genomeCount, 0, 0, 0, 0, mutationProbability, mutationStrengthStdDev, 0, 0);

    const float initialValue = 1.0f;
    Genome g({ initialValue });
    lab.mutate(g);

    QVERIFY(g.at(0) != initialValue);
}

void TestGeneraticAlgorithm::lab_reproduce_must_use_random_parent_genes()
{
    const int genomeCount = 1;
    const float minGeneValue = 0.f;
    const float maxGeneValue = 1e9f;
    const float valueA = 1.0f, valueB = 2.0f;
    Genome a({ valueA }), b({ valueB });

    const float crossOverNever = 0.0f;
    Genome resultA = GenomeLab(genomeCount, 0, 0, minGeneValue, maxGeneValue, 0, 0, 0, 0, crossOverNever, crossOverNever).reproduce(a, b);
    QVERIFY(resultA == a);

    const float crossOverAlways = 1.0f;
    Genome resultB = GenomeLab(genomeCount, 0, 0, minGeneValue, maxGeneValue, 0, 0, 0, 0, crossOverAlways, crossOverAlways).reproduce(a, b);
    QVERIFY(resultB == b);
}

class TestEvaluator : public IEvaluator {
public:
    TestEvaluator(double target)
        : _target(target)
    {
    }
    virtual ~TestEvaluator() override { }
    virtual float evaluate(Genome genome) const override
    {
        double sum = 0;
        for (auto gene : genome) {
            sum += gene;
        }
        double error = _target - sum;
        return static_cast<float>(sqrt(error * error));
    }

private:
    double _target;
};

void TestGeneraticAlgorithm::generatic_algorithm_must_converge()
{
    const int genomeCount = 2;
    const float minGeneInit = 0.f;
    const float maxGeneInit = 1.f;
    const float minGeneValue = 0.f;
    const float maxGeneValue = 1e9f;
    const float mutationProbability = 0.33f;
    const float mutationStrengthStdDev = 1.2f;
    GenomeLab lab(genomeCount, minGeneInit, maxGeneInit, minGeneValue, maxGeneValue, mutationProbability, mutationStrengthStdDev, 0, 0);
    TestEvaluator evaluator(42);

    const int populationSize = 200;
    const float elitePortion = 0.02f;
    float rankSelectionMultiplier = 10.0f;
    GeneticAlgorithm ga(lab, evaluator, populationSize, elitePortion, rankSelectionMultiplier);

    for (int generation = 0; generation < 1000; ++generation) {
        float lowestError = ga.evaluateAll();
        qDebug() << "Generation:" << generation << ", Lowest error:" << lowestError << ", Genome:" << ga.genomes().first();

        if (lowestError < 0.01)
            return;
        ga.nextGeneration();
    }
    QFAIL("Did not converge after maximum amount of generations!");
}
