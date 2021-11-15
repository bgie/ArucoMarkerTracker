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
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>

namespace {
const QStringList filter = QStringList() << (QStringLiteral("????????.JPG"));
const QString FRAMES(QStringLiteral("frames"));
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
    qDeleteAll(_frames);
    _frames.clear();

    QDir dir(path);
    for (QString filename : dir.entryList(filter, QDir::Files | QDir::Readable, QDir::Name)) {
        _frames.append(new Frame(dir, filename));
    }
    emit framesChanged();
}

void Video::load(QString path, QString filename)
{
    qDeleteAll(_frames);
    _frames.clear();

    QFile file(QDir(path).filePath(filename));
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();
        QJsonArray arr = obj[FRAMES].toArray();
        for (auto it = arr.constBegin(); it != arr.constEnd(); ++it) {
            _frames.append(new Frame(path, (*it).toObject()));
        }
    }
    emit framesChanged();
}

bool Video::save(QString path, QString filename) const
{
    QJsonObject obj;
    QJsonArray arr;
    for (auto f : _frames) {
        arr.append(f->toJson());
    }
    obj.insert(FRAMES, arr);
    QFile file(QDir(path).absoluteFilePath(filename));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
        return true;
    }
    return false;
}

QList<QObject*> Video::framesQObjects() const
{
    QList<QObject*> results;
    for (auto f : _frames) {
        results.append(f);
    }
    return results;
}

QList<Frame*> Video::frames() const
{
    return _frames;
}
