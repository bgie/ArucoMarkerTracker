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
#ifndef TESTGENERATICALGORITHM_H
#define TESTGENERATICALGORITHM_H

#include <QObject>

class TestGeneraticAlgorithm : public QObject
{
    Q_OBJECT
private slots:
    void lab_biogenesis_must_create_valid_genome();
    void lab_mutate_must_mutate();
    void lab_reproduce_must_use_random_parent_genes();
    void generatic_algorithm_must_converge();
};

#endif // TESTGENERATICALGORITHM_H
