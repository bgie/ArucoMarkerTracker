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
import MyApp 1.0
import './controls/'

Rectangle {
    property MainController controller

    height: cameraSettingsGrid.implicitHeight + 2 * Style.mediumMargin
    width: cameraSettingsGrid.implicitWidth + 2 * Style.largeMargin

    color: Style.darkerGray

    GridLayout {
        id: cameraSettingsGrid
        anchors.centerIn: parent
        rows: 2
        flow: GridLayout.TopToBottom
        columnSpacing: Style.mediumMargin
        rowSpacing: Style.smallMargin

        MyLabel {
            text: "Camera"
            verticalAlignment: Qt.AlignVCenter
            height: parent.height
        }

        MyTextEdit {
            Layout.preferredWidth: 160
            Layout.leftMargin: Style.mediumMargin
            text: controller.videoDevice
            onTextChanged: controller.videoDevice = text
            enabled: !controller.isCameraStreaming
        }

        MyLabel {
            text: "Format"
        }
        MyComboBox {
            enabled: !controller.isCameraStreaming
            model: controller.videoFormats
            Layout.preferredWidth: 380
            Layout.leftMargin: Style.mediumMargin
            currentIndex: controller.currentVideoFormatIndex
            onCurrentIndexChanged: controller.setCurrentVideoFormatIndex(currentIndex)
        }
        MyLabel {
            text: "Gain"
        }
        MyTextEdit {
            Layout.leftMargin: Style.mediumMargin
            Layout.preferredWidth: 60
//                    text: controller.exposure
//                    onTextChanged: controller.exposure = parseInt(text)
        }
        MyLabel {
            text: "Exposure"
        }
        MyTextEdit {
            Layout.leftMargin: Style.mediumMargin
            Layout.preferredWidth: 60
            text: controller.exposure
            onTextChanged: controller.exposure = parseInt(text)
        }

        MyLabel {
            text: "Streaming"
        }
        Row {
            Layout.leftMargin: Style.mediumMargin
            MyButton {
                id: startCameraButton
                text: "Start"
                backgroundColor: Style.darkGray
                enabled: controller.canCameraStream && !controller.isCameraStreaming && !controller.isReplayStreaming
                visible: !controller.isCameraStreaming
                onClicked: controller.startCameraStream()
            }
            MyButton {
                text: "Stop"
                backgroundColor: Style.darkGray
                visible: controller.isCameraStreaming
                onClicked: controller.stopCameraStream()
                width: startCameraButton.width
            }
        }
    }
}
