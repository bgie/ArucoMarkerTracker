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
import MyApp 1.0

Button {
    property alias radius: backgroundRect.radius
    property color backgroundColor: Style.darkerGray

    id: control
    font.pointSize: Style.fontSize
    hoverEnabled: true

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.hovered ? Style.black : Style.white
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        leftPadding: 10
        rightPadding: 10
        topPadding: 5
        bottomPadding: 5
        y: control.down ? 7 : 5
    }

    background: Rectangle {
        id: backgroundRect
        radius: Style.smallRadius
        color: control.down ? Style.white :
               control.hovered ? Style.lightGray : control.backgroundColor
    }
}
