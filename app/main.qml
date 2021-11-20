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

Window {
    id: window
    width: 1700
    height: 800
    visible: true
    visibility: Window.Windowed
    // visibility: Window.FullScreen
    title: qsTr("Aruco tracker")
    color: Style.darkGray

    MainController {
        id: controller
    }

    Rectangle {
        id: centerImage
        anchors.left: leftPanel.right
        anchors.top: topPanel.bottom
        anchors.bottom: parent.bottom
        anchors.right: rightPanel.left
        color: Style.black        

        ImageItem {
            anchors.fill: parent
            anchors.margins: 2
            image: controller.image
        }
    }

    Column {
        id: topPanel
        anchors.left: parent.left
        anchors.right: rightPanel.left
        anchors.top: parent.top
        padding: Style.smallMargin
        bottomPadding: Style.largeMargin
        spacing: Style.smallMargin

        height: expandTopPanel.expanded ? implicitHeight : 0
        visible: expandTopPanel.expanded

        Row {
            spacing: Style.smallMargin

            MyLabel {
                text: "Camera"
                verticalAlignment: Qt.AlignVCenter
                height: parent.height
            }
            MyTextEdit {
                width: 160
                text: controller.videoDevice
                onTextChanged: controller.videoDevice = text
                enabled: !controller.isCameraStreaming
            }
            MyButton {
                id: connectButton
                text: "Connect"
                enabled: controller.connectPossible && !controller.isCameraStreaming
                onClicked: controller.connect()
            }
            MyLabel {
                text: "Format"
                verticalAlignment: Qt.AlignVCenter
                height: parent.height
            }
            MyComboBox {
                enabled: !controller.isCameraStreaming
                model: controller.videoFormats
                width: 380
                currentIndex: controller.currentVideoFormatIndex
                onCurrentIndexChanged: controller.setCurrentVideoFormatIndex(currentIndex)
            }
            MyButton {
                id: startCameraButton
                text: "Start"
                enabled: controller.canCameraStream && !controller.isCameraStreaming && !controller.isReplayStreaming
                visible: !controller.isCameraStreaming
                onClicked: controller.startCameraStream()
            }
            MyButton {
                text: "Stop"
                visible: controller.isCameraStreaming
                onClicked: controller.stopCameraStream()
                width: startCameraButton.width
            }
            MyLabel {
                text: "Exposure"
                verticalAlignment: Qt.AlignVCenter
                height: parent.height
            }
            MyTextEdit {
                width: 60
                text: controller.exposure
                onTextChanged: controller.exposure = parseInt(text)
            }
        }
        Row {
            spacing: Style.smallMargin
            MyCheckBox {
                id: saveToDiskCheckBox
                text: "Save to folder"
                height: parent.height
                checked: controller.saveallframesEnabled
                onCheckedChanged: controller.saveallframesEnabled = checked
            }
            MyTextEdit {
                enabled: !saveToDiskCheckBox.checked
                width: 400
                text: controller.savePath
                onTextChanged: controller.savePath = text
            }
            MyLabel {
                text: "Skip frames"
                verticalAlignment: Qt.AlignVCenter
                height: parent.height
            }
            MyTextEdit {
                width: 60
                text: controller.skipSavingFrames
                onTextChanged: controller.skipSavingFrames = parseInt(text)
            }
            MyButton {
                text: "Save single frame"
                enabled: !controller.saveallframesEnabled
                onClicked: controller.saveSingleFrame()
            }
        }
    }

    Item {
        id: leftPanel
        anchors.left: parent.left
        anchors.top: topPanel.bottom
        anchors.bottom: parent.bottom
        width: expandLeftPanel.expanded ? headerRow.width + Style.largeMargin + 3*Style.smallMargin : 0
        visible: expandLeftPanel.expanded

        Rectangle {
            id: header
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.leftMargin: Style.smallMargin
            anchors.rightMargin: Style.largeMargin
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
                MyLabel {
                    id: headerDetectedMarkers
                    text: "Markers"
                    horizontalAlignment: Qt.AlignHCenter
                }
            }
        }
        ListView {
            anchors.left: parent.left
            anchors.top: header.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: Style.smallMargin
            anchors.rightMargin: Style.largeMargin
            model: controller.video.frames
            clip: true

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
                        text: modelData.fileName
                        color: highlighted ? Style.black : Style.lightGray
                    }
                    MyLabel {
                        width: headerCalibrationRMSError.width
                        text: modelData.chessBoardReprojectionError
                        color: highlighted ? Style.black : Style.lightGray
                        horizontalAlignment: Qt.AlignHCenter
                    }
                    MyLabel {
                        width: headerDetectedMarkers.width
                        text: modelData.markers.length
                        color: highlighted ? Style.black : Style.lightGray
                        horizontalAlignment: Qt.AlignHCenter
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: controller.setFrameIndex(index)
                }
            }
        }
    }

    Item {
        id: rightPanel
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: expandRightPanel.expanded ? 500 : 0
        visible: expandRightPanel.expanded

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Style.smallMargin
            anchors.leftMargin: Style.largeMargin
            spacing: Style.smallMargin

            MyLabel {
                text: "Replay from folder"
            }
            Row {
                leftPadding: Style.smallMargin
                MyTextEdit {
                    width: 400
                    text: controller.loadPath
                    onTextChanged: controller.loadPath = text
                    enabled: !controller.isReplayStreaming
                }
            }
            Row {
                spacing: Style.smallMargin
                MyLabel {
                    text: "Auto play"
                    verticalAlignment: Qt.AlignVCenter
                    height: parent.height
                }
                MyButton {
                    id: startReplayButton
                    text: "Start"
                    enabled: controller.canReplayStream && !controller.isReplayStreaming && !controller.isCameraStreaming
                    visible: !controller.isReplayStreaming
                    onClicked: controller.startReplayStream()
                }
                MyButton {
                    text: "Stop"
                    visible: controller.isReplayStreaming
                    onClicked: controller.stopReplayStream()
                    width: startReplayButton.width
                }
                MyLabel {
                    text: "Fps"
                    verticalAlignment: Qt.AlignVCenter
                    height: parent.height
                }
                MyTextEdit {
                    width: 60
                    text: controller.replayFps
                    onTextChanged: controller.replayFps = text
                    enabled: !controller.isReplayStreaming
                }
            }
            Row {
                spacing: Style.smallMargin
                MyButton {
                    text: "|←"
                    onClicked: controller.frameFirst()
                }
                MyButton {
                    text: "←"
                    onClicked: controller.framePrevious()
                }
                MyButton {
                    text: "→"
                    onClicked: controller.frameNext()
                }
                MyLabel {
                    text: controller.frameIndexString
                    verticalAlignment: Qt.AlignVCenter
                    height: parent.height
                }
            }
            Row {
                spacing: Style.smallMargin
                MyLabel {
                    text: "Current file"
                    verticalAlignment: Qt.AlignVCenter
                    height: parent.height
                }
                MyLabel {
                    text: controller.frameFileName
                    verticalAlignment: Qt.AlignVCenter
                    height: parent.height
                }
                MyButton {
                    text: "DELETE"
                    enabled: controller.frameFileName.length
                    onClicked: controller.deleteFrame()
                }
            }
            Row {
                spacing: Style.smallMargin
                MyButton {
                    text: "Calibrate using all"
                    onClicked: controller.calibrateUsingAll()
                }
                MyLabel {
                    text: controller.calibrationResultString
                    verticalAlignment: Qt.AlignVCenter
                    height: parent.height
                }
            }
            Row {
                spacing: Style.smallMargin
                MyLabel {
                    text: "CameraCalibration.json"
                    verticalAlignment: Qt.AlignVCenter
                    height: parent.height
                }
                MyButton {
                    text: "Load"
                    onClicked: controller.loadCalibration()
                }
                MyButton {
                    text: "Save"
                    onClicked: controller.saveCalibration()
                }
            }
            MyButton {
                text: "Detect aruco in all"
                onClicked: controller.detectArucoInAll()
            }
            Row {
                spacing: Style.smallMargin
                MyLabel {
                    text: "Video.json"
                    verticalAlignment: Qt.AlignVCenter
                    height: parent.height
                }
                MyButton {
                    text: "Load"
                    onClicked: controller.loadVideo()
                }
                MyButton {
                    text: "Save"
                    onClicked: controller.saveVideo()
                }
            }

            MyCheckBox {
                text: "Show detected checkerboard"
                checked: controller.detectCheckerBoardEnabled
                onCheckedChanged: controller.detectCheckerBoardEnabled = checked
            }
            MyCheckBox {
                text: "Apply calibration"
                checked: controller.applyCalibrationEnabled
                onCheckedChanged: controller.applyCalibrationEnabled = checked
            }
            MyCheckBox {
                text: "Show detected aruco markers"
                checked: controller.detectArucoEnabled
                onCheckedChanged: controller.detectArucoEnabled = checked
            }
            MyCheckBox {
                text: "Kalman filtered tracking"
                checked: controller.kallmanFilterEnabled
                onCheckedChanged: controller.kallmanFilterEnabled = checked
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: controller.frame ? controller.frame.markers : {}
                clip: true

                delegate: Rectangle {
                    color: index % 2 ? Style.darkerGray : Style.darkGray
                    height: markerRow.implicitHeight + Style.smallMargin
                    width: parent.width

                    Row {
                        id: markerRow
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: Style.smallMargin
                        spacing: 4
                        MyLabel {
                            width: 60
                            text: "id: " + modelData.id
                        }
                        MyLabel {
                            width: 100
                            text: "x: " + modelData.position.x.toFixed(2)
                        }
                        MyLabel {
                            width: 100
                            text: "y: " + modelData.position.y.toFixed(2)
                        }
                        MyLabel {
                            width: 100
                            text: "z: " + modelData.position.z.toFixed(2)
                        }
                    }
                }
            }
        }
    }

    MyButton {
        property bool expanded: true

        id: expandTopPanel
        anchors.horizontalCenter: topPanel.horizontalCenter
        anchors.verticalCenter: topPanel.bottom

        text: expanded ? "↑" : "↓"
        width: height
        radius: height / 2

        onClicked: expanded = !expanded
    }


    MyButton {
        property bool expanded: true

        id: expandLeftPanel
        anchors.horizontalCenter: leftPanel.right
        anchors.verticalCenter: leftPanel.verticalCenter

        text: expanded ? "←" : "→"
        width: height
        radius: height / 2

        onClicked: expanded = !expanded
    }

    MyButton {
        property bool expanded: true

        id: expandRightPanel
        anchors.horizontalCenter: rightPanel.left
        anchors.verticalCenter: rightPanel.verticalCenter

        text: expanded ? "→" : "←"
        width: height
        radius: height / 2

        onClicked: expanded = !expanded
    }

    MyLabel {
        anchors.right: centerImage.right
        anchors.bottom: centerImage.bottom
        anchors.margins: Style.smallMargin

        text: controller.fps.toFixed(2) + " fps"
        visible: controller.isCameraStreaming || controller.isReplayStreaming
    }

    MyButton {
        visible: visibility == Window.FullScreen
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Style.smallMargin
        backgroundColor: "#600000"

        text: "X"
        width: height
        radius: height / 2

        onClicked: window.close()
    }
}
