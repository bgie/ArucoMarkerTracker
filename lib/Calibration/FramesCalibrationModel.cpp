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
#include "FramesCalibrationModel.h"
#include "Video/Frame.h"
#include "Video/Video.h"

namespace {
const int ROLE_FILENAME = Qt::UserRole;
}

FramesCalibrationModel::FramesCalibrationModel(QObject* parent)
    : QAbstractListModel(parent)
    , _video(nullptr)
{
}

void FramesCalibrationModel::setVideo(Video* video)
{
    if (_video != video) {
        if (_video) {
            disconnect(_video, 0, this, 0);
        }

        beginResetModel();
        _video = video;
        endResetModel();

        if (_video) {
            connect(_video, &Video::framesChanging, this, &FramesCalibrationModel::beginResetModel);
            connect(_video, &Video::framesChanged, this, &FramesCalibrationModel::endResetModel);
        }
    }
}

Frame* FramesCalibrationModel::frameAtRow(int row) const
{
    if (!_video)
        return nullptr;
    return _video->frames().value(row, nullptr);
}

int FramesCalibrationModel::rowCount(const QModelIndex&) const
{
    return _video->frames().size();
}

QVariant FramesCalibrationModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();
    if (index.isValid() && row < _video->frames().size()) {
        switch (role) {
        case ROLE_FILENAME:
            return QVariant(_video->frames().at(row)->fileName());
        }
    }
    return QVariant();
}

QHash<int, QByteArray> FramesCalibrationModel::roleNames() const
{
    static QHash<int, QByteArray> roles({ { ROLE_FILENAME, QByteArray("fileName") } });
    return roles;
}
