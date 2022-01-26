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
#pragma once
#include <QAbstractListModel>

class Video;
class Frame;

class FramesCalibrationModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit FramesCalibrationModel(QObject *parent = nullptr);

    void setVideo(Video* video);
    Frame* frameAtRow(int row) const;

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

private:
    Video* _video;
};
