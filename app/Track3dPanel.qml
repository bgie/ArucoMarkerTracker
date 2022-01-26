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
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import ArucoMarkerTracker 1.0
import './controls/'

Item {
    Track3dController {
        id: controller
        videoSource: globalVideoSource
        aruco: globalAruco
    }

    ImageItem {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: sidePanel.left
        anchors.rightMargin: Style.smallMargin

        image: controller.image

        MyLabel {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: Style.smallMargin

            text: controller.fps.toFixed(2) + " fps"
        }
    }

    ColumnLayout {
        id: sidePanel
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 320
        anchors.margins: Style.smallMargin
        spacing: Style.smallMargin

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Rectangle {
                id: header
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 2
                color: Style.darkerGray
                height: headerRow.implicitHeight + 2 * Style.smallMargin
                Row {
                    id: headerRow
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: Style.smallMargin
                    spacing: Style.smallMargin
                    MyLabel {
                        id: headerId
                        width: 60
                        text: "Id"
                        horizontalAlignment: Qt.AlignHCenter
                    }
                    MyLabel {
                        id: headerX
                        width: 60
                        text: "x"
                        horizontalAlignment: Qt.AlignHCenter
                    }
                    MyLabel {
                        id: headerY
                        width: 60
                        text: "y"
                        horizontalAlignment: Qt.AlignHCenter
                    }
                    MyLabel {
                        id: headerZ
                        width: 60
                        text: "z"
                        horizontalAlignment: Qt.AlignHCenter
                    }
                }
            }
            ListView {
                anchors.left: parent.left
                anchors.top: header.bottom
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 2
                clip: true
                model: controller.markers

                delegate: Rectangle {
                    property bool highlighted: index === controller.markerIndex
                    color: highlighted ? Style.lightGray : index % 2 ? Style.darkerGray : Style.darkGray
                    height: delegateRow.implicitHeight + 2 * Style.smallMargin
                    width: parent.width
                    Row {
                        id: delegateRow
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: Style.smallMargin
                        spacing: Style.smallMargin
                        MyLabel {
                            width: headerId.width
                            text: modelData.id
                            color: highlighted ? Style.black : Style.lightGray
                        }
                        MyLabel {
                            width: headerX.width
                            text: modelData.x
                            color: highlighted ? Style.black : Style.lightGray
                        }
                        MyLabel {
                            width: headerY.width
                            text: modelData.y
                            color: highlighted ? Style.black : Style.lightGray
                        }
                        MyLabel {
                            width: headerZ.width
                            text: modelData.z
                            color: highlighted ? Style.black : Style.lightGray
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: controller.frameIndex = index
                    }
                }
            }
        }
    }
}
