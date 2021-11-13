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
#ifndef IMAGESAVER_H
#define IMAGESAVER_H

#include <QImage>
#include <QObject>

class ImageSaver : public QObject
{
    Q_OBJECT /*  ArucoMarkerTracker
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

        public : explicit ImageSaver(QObject* parent = nullptr);

    void setPath(QString path);
    void setEnabled(bool enabled);

public slots:
    void saveImage(QImage img);
    void saveSingleImage(QImage img);

private:
    QString generateFilename();
    void findFirstAvailableFileCounter();
    void saveImageImpl(QImage img, bool saveSingleFile);

private:
    QString _path;
    bool _enabled;
    int _fileCounter;
};

#endif // IMAGESAVER_H
