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
#include "Video.h"
#include "Frame.h"
#include <QDir>

namespace {
const QStringList filter = QStringList() << (QStringLiteral("????????.JPG"));
}

Video::Video(QObject *parent) : QObject(parent)
{
}

Video::~Video()
{
    qDeleteAll(_frames);
}

void Video::load(QString path)
{
    emit framesChanging();

    qDeleteAll(_frames);
    _frames.clear();

    QDir dir(path);
    for (QString filename : dir.entryList(filter, QDir::Files | QDir::Readable, QDir::Name)) {
        _frames.append(new Frame(dir, filename));
    }
    emit framesChanged();
}

QList<Frame*> Video::frames() const
{
    return _frames;
}
