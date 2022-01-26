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
#ifndef ARUCO_H
#define ARUCO_H

#include <QImage>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <opencv2/core/mat.hpp>
#include <vector>

class Aruco : public QObject {
    Q_OBJECT

public:
    struct Markers {
        std::vector<std::vector<cv::Point2f>> corners;
        std::vector<int> ids;
        std::vector<cv::Vec3d> rvecs, tvecs;
    };

public:
    explicit Aruco(QObject* parent = nullptr);
    virtual ~Aruco();

    void setCameraMatrix(cv::Mat cameraMatrix, cv::Mat distCoeffs);
    Markers detectMarkers(QImage image) const;
    void drawMarkers(QImage& image, const Markers& markers) const;

    void generateMarkerImageFiles(QString path) const;

private:
    struct Data;
    QScopedPointer<Data> _d;
};

#endif // ARUCO_H
