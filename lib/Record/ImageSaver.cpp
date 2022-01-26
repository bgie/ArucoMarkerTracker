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
#include "ImageSaver.h"
#include <QDir>
#include <QtConcurrent/QtConcurrentRun>

namespace {
const QString format(QStringLiteral("%1.JPG"));
}

ImageSaver::ImageSaver(QObject* parent)
    : QObject(parent)
    , _enabled(false)
    , _fileCounter(0)
{
}

void ImageSaver::setPath(QString path)
{
    if (_path != path) {
        _path = path;

        findFirstAvailableFileCounter();
    }
}

void ImageSaver::setEnabled(bool enabled)
{
    if (enabled != _enabled) {
        _enabled = enabled;

        findFirstAvailableFileCounter();
    }
}

void ImageSaver::saveImage(QImage img)
{
    saveImageImpl(img, false);
}

void ImageSaver::saveSingleImage(QImage img)
{
    findFirstAvailableFileCounter();
    saveImageImpl(img, true);
}

void ImageSaver::saveImageImpl(QImage img, bool saveSingleFile)
{
    if ((_enabled || saveSingleFile) && QDir(_path).mkpath(QString("."))) {
        QString filename = generateFilename();
        _fileCounter++;
        QtConcurrent::run([=]() -> void {
            img.save(filename, "JPG", 100);
        });
    }
}

QString ImageSaver::generateFilename()
{
    return QDir(_path).absoluteFilePath(format.arg(_fileCounter, 8, 10, QChar('0')));
}

void ImageSaver::findFirstAvailableFileCounter()
{
    _fileCounter = -1;
    QString filename;
    do {
        _fileCounter++;
        filename = generateFilename();
    } while (QFile::exists(filename));
}
