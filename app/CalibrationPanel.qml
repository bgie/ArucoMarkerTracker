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
    CalibrationController {
        id: controller
        aruco: globalAruco
    }

    ColumnLayout {
        id: leftPanel
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: Style.smallMargin
        spacing: Style.smallMargin

        MyLabel {
            text: "Calibration images folder"
        }
        MyTextEdit {
            width: 400
            Layout.leftMargin: Style.mediumMargin
            text: controller.loadPath
            onTextChanged: controller.loadPath = text
        }
        MyButton {
            id: calculateButton
            text: "Calibrate"
            onClicked: controller.calibrate()
        }
        MyLabel {
            Layout.leftMargin: Style.mediumMargin
            text: controller.calibrationValues;
        }
        MyLabel {
            Layout.leftMargin: Style.mediumMargin
            text: controller.totalError;
        }

        Row {
            spacing: Style.smallMargin
            MyLabel {
                text: controller.saveFilename
                verticalAlignment: Qt.AlignVCenter
                height: parent.height
            }
            MyButton {
                text: "Load"
                onClicked: controller.loadCalibration()
            }
            MyButton {
                text: "Save"
                enabled: controller.calibrationValues.length > 0
                onClicked: controller.saveCalibration()
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 600

            color: Style.black

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
                        id: headerFilename
                        width: 160
                        text: "Filename"
                        horizontalAlignment: Qt.AlignHCenter
                    }
                    MyLabel {
                        id: headerCalibrationRMSError
                        text: "Calib error"
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
                model: controller.model

                delegate: Rectangle {
                    property bool highlighted: index === controller.frameIndex
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
                            width: headerFilename.width
                            text: fileName
                            color: highlighted ? Style.black : Style.lightGray
                        }
                        MyLabel {
                            width: headerCalibrationRMSError.width
                            // text: modelData.chessBoardReprojectionError
                            color: highlighted ? Style.black : Style.lightGray
                            horizontalAlignment: Qt.AlignHCenter
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

    ImageItem {
        anchors.left: leftPanel.right
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Style.smallMargin

        image: controller.image
    }
}
