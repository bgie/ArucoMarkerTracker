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
pragma Singleton
import QtQuick 2.0

Item {
    readonly property int windowMargins: 20
    readonly property int smallMargin: 10
    readonly property int largeMargin: 30

    readonly property int smallRadius: 3

    readonly property string fontSize: "14"

    readonly property color black: "#000000"
    readonly property color darkerGray: "#202020"
    readonly property color darkGray: "#303030"
    readonly property color mediumGray: "#808080"
    readonly property color lightGray: "#D0D0D0"
    readonly property color white: "#FFFFFF"
}
