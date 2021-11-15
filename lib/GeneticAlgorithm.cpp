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
#include <QtConcurrent/QtConcurrentMap>
#include <algorithm>
#include <chrono>
#include <functional>
#include <iterator>
#include <math.h>
#include <random>
#include <vector>

using namespace std;

mt19937* randomNumberGenerator()
{
    static mt19937 generator(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    return &generator;
}

float randomUniformFloat(float minInclusive, float maxInclusive)
{
    uniform_real_distribution<float> distribution(minInclusive, maxInclusive);
    return distribution(*randomNumberGenerator());
}

float randomGaussianFloat(float mean, float stddev)
{
    normal_distribution<float> distribution(mean, stddev);
    return distribution(*randomNumberGenerator());
}

bool randomBool(double probabilityOfTrue)
{
    std::bernoulli_distribution distribution(probabilityOfTrue);
    return distribution(*randomNumberGenerator());
}

GenomeLab::GenomeLab(int genomeSize,
    float initGeneMin,
    float initGeneMax,
    float clipGeneMin,
    float clipGeneMax,
    float mutationMultiplyProb,
    float mutationMultiplyStdDev,
    float mutationAddProb,
    float mutationAddStdDev,
    float crossOverMinAmount,
    float crossOverMaxAmount)
    : _genomeSize(genomeSize)
    , _initGeneMin(initGeneMin)
    , _initGeneMax(initGeneMax)
    , _clipGeneMin(clipGeneMin)
    , _clipGeneMax(clipGeneMax)
    , _mutationMultiplyProb(mutationMultiplyProb)
    , _mutationMultiplyStdDev(mutationMultiplyStdDev)
    , _mutationAddProb(mutationAddProb)
    , _mutationAddStdDev(mutationAddStdDev)
    , _crossOverMinAmount(crossOverMinAmount)
    , _crossOverMaxAmount(crossOverMaxAmount)
{
}

Genome GenomeLab::biogenesis() const
{
    Genome genome(_genomeSize);
    for (int i = 0; i < _genomeSize; ++i) {
        genome[i] = randomUniformFloat(_initGeneMin, _initGeneMax);
    }
    return genome;
}

Genome& GenomeLab::mutate(Genome& victim) const
{
    for (int i = 0; i < victim.size(); ++i) {
        if (randomBool(_mutationMultiplyProb)) {
            float value = randomGaussianFloat(0.0f, _mutationMultiplyStdDev);
            if (value >= 0.0f) {
                victim[i] *= 1 + value;
            } else {
                victim[i] /= 1 - value;
            }
        }
        if (randomBool(_mutationAddProb)) {
            victim[i] += randomGaussianFloat(0.0f, _mutationAddStdDev);
        }
    }
    return victim;
}

Genome GenomeLab::reproduce(const Genome firstParent, const Genome otherParent) const
{
    float amountOfCrossOver = randomUniformFloat(_crossOverMinAmount, _crossOverMaxAmount);
    Genome genome(_genomeSize);
    for (int i = 0; i < _genomeSize; ++i) {
        bool useOtherParent = amountOfCrossOver > 0.0f && randomBool(amountOfCrossOver);
        genome[i] = useOtherParent ? otherParent.at(i) : firstParent.at(i);
    }
    return genome;
}

Genome& GenomeLab::clip(Genome& victim) const
{
    for (int i = 0; i < victim.size(); ++i) {
        *reinterpret_cast<uint32_t*>(&victim[i]) &= 0xFFC00000;

        if (victim[i] < _clipGeneMin) {
            victim[i] = _clipGeneMin;
        } else if (victim[i] > _clipGeneMax) {
            victim[i] = _clipGeneMax;
        }
    }
    return victim;
}

IEvaluator::~IEvaluator()
{
}

GeneticAlgorithm::GeneticAlgorithm(const GenomeLab& lab, const IEvaluator& evaluator, int populationSize, float elitePortion, float rankSelectionMultiplier)
    : _lab(lab)
    , _evaluator(evaluator)
    , _populationSize(populationSize)
    , _elitePortion(elitePortion)
    , _rankSelectionMultiplier(rankSelectionMultiplier)
{
    for (int i = 0; i < _populationSize; ++i) {
        Genome g = lab.biogenesis();
        _population << lab.clip(g);
    }
}

float GeneticAlgorithm::evaluateAll()
{
    _fitness = QtConcurrent::mapped(_population, std::bind(&GeneticAlgorithm::evaluate, this, std::placeholders::_1)).results();
    //    _fitness.clear();
    //    for (auto g : _population) {
    //        _fitness << evaluate(g);
    //    }

    std::sort(_fitness.begin(), _fitness.end(), [](const QPair<float, Genome>& a, const QPair<float, Genome>& b) -> bool {
        return a.first < b.first;
    });
    return _fitness.at(0).first;
}

void GeneticAlgorithm::nextGeneration()
{
    // generate a roulette wheel for proportional selection by fitness rank
    QVector<float> rouletteWheel(_populationSize);
    float totalRank = 0;
    for (int i = 0; i < _populationSize; ++i) {
        float rankBonus = (_rankSelectionMultiplier - 1) * (_populationSize - i - 1) / (_populationSize - 1);
        totalRank += 1 + rankBonus;
        rouletteWheel[i] = totalRank;
    }

    // preserve elites without mutation/crossover
    int eliteCount = min(_populationSize, max(0, static_cast<int>(ceil(_elitePortion * _populationSize))));
    for (int i = 0; i < eliteCount; ++i) {
        _population[i] = _fitness.at(i).second;
    }
    // create other genomes
    for (int i = eliteCount; i < _populationSize; ++i) {
        // get 2 random parents
        QVector<Genome> parents;
        while (parents.size() < 2) {
            float selectionPoint = randomUniformFloat(0, totalRank);
            auto it = lower_bound(rouletteWheel.constBegin(), rouletteWheel.constEnd(), selectionPoint);
            int index = it - rouletteWheel.begin();
            parents << _fitness.at(index).second;
        }
        Genome child = _lab.reproduce(parents.at(0), parents.at(1));
        _population[i] = _lab.clip(_lab.mutate(child));
    }
    _fitness.clear();
}

QList<Genome> GeneticAlgorithm::genomes() const
{
    return _population;
}

void GeneticAlgorithm::seed(QList<Genome> seeds)
{
    int count = qMin(seeds.size(), _populationSize);
    for (int i = 0; i < count; ++i) {
        Genome g = seeds.at(i);
        _population[i] = _lab.clip(g);
    }
}

QPair<float, Genome> GeneticAlgorithm::evaluate(Genome g) const
{
    //    float worstError = _evaluator.evaluate(g);
    //    for (int i = 0; i < 15; ++i) {
    //        float error = _evaluator.evaluate(g);
    //        if (error > worstError)
    //            worstError = error;
    //    }
    //    return QPair<float, Genome>(worstError, g);
    return QPair<float, Genome>(_evaluator.evaluate(g), g);
}
