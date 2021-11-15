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
#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <QVector>

typedef QVector<float> Genome;

float randomUniformFloat(float minInclusive, float maxInclusive);
float randomGaussianFloat(float mean, float stddev);
bool randomBool(double probabilityOfTrue);

class GenomeLab {
public:
    GenomeLab(
        int genomeSize,
        float initGeneMin,
        float initGeneMax,
        float clipGeneMin,
        float clipGeneMax,
        float mutationMultiplyProb,
        float _mutationMultiplyStdDev,
        float mutationAddProb,
        float mutationAddStdDev,
        float crossOverMinAmount = 0.0f,
        float crossOverMaxAmount = 0.5f);

    Genome biogenesis() const;
    Genome& mutate(Genome& victim) const;
    Genome reproduce(const Genome firstParent, const Genome otherParent) const;
    Genome& clip(Genome& victim) const;

private:
    int _genomeSize;
    float _initGeneMin;
    float _initGeneMax;
    float _clipGeneMin;
    float _clipGeneMax;
    float _mutationMultiplyProb;
    float _mutationMultiplyStdDev;
    float _mutationAddProb;
    float _mutationAddStdDev;
    float _crossOverMinAmount;
    float _crossOverMaxAmount;
};

class IEvaluator {
public:
    virtual ~IEvaluator();
    virtual float evaluate(Genome genome) const = 0;
};

class GeneticAlgorithm {
public:
    GeneticAlgorithm(const GenomeLab& lab, const IEvaluator& evaluator, int populationSize, float elitePortion, float rankSelectionMultiplier);

    float evaluateAll();
    void nextGeneration();

    QList<Genome> genomes() const;
    void seed(QList<Genome> seeds);

private:
    QPair<float, Genome> evaluate(Genome g) const;

private:
    const GenomeLab& _lab;
    const IEvaluator& _evaluator;
    int _populationSize;
    float _elitePortion;
    float _rankSelectionMultiplier;
    QList<Genome> _population;
    QList<QPair<float, Genome>> _fitness;
};

#endif // GENETICALGORITHM_H
