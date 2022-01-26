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
#include <QImage>
#include <QQuickItem>

class ImageItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit ImageItem(QQuickItem* parent = nullptr);
    virtual ~ImageItem() override;

    virtual QSGNode* updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* updatePaintNodeData) override;
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
    QImage image() const;

public slots:
    void setImage(QImage image);

signals:
    void imageChanged(QImage image);

private:
    QImage _image;
    bool _imageChanged;
};
