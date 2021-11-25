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
import QtQuick 2.0
import MyApp 1.0

Rectangle {
    property alias text: textEdit.text

    color: Style.lightGray
    border.color: textEdit.activeFocus ? Style.white : Style.lightGray
    border.width: 3
    radius: Style.smallRadius
    height: textEdit.contentHeight + 20

    TextEdit {
        id: textEdit
        anchors.fill: parent
        anchors.margins: 10
        font.pointSize: Style.fontSize
        color: enabled ? Style.black : Style.mediumGray
        selectByMouse: true
    }
}
