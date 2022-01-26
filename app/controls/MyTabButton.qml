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
import QtQuick 2.12
import QtQuick.Controls 2.12
import ArucoMarkerTracker 1.0

Button {
    property alias radius: backgroundRect.radius
    property color backgroundColor: Style.darkerGray
    property int tabIndex
    property bool selected

    id: control
    font.pointSize: Style.fontSize
    hoverEnabled: true

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: selected ? Style.white : control.hovered ? Style.lightGray : Style.black
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        leftPadding: 15
        rightPadding: 15
        topPadding: 10
        bottomPadding: 10
        transform: Translate { y: control.down ? 2 : 0 }
    }

    background: Rectangle {
        id: backgroundRect
        radius: Style.largeRadius
        color: control.down ? Style.mediumGray :
               control.hovered || selected ? Style.darkGray : control.backgroundColor

        Rectangle {
            color: parent.color
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: parent.radius
        }
    }
}
