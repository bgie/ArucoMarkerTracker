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

Rectangle {
    property ReplayController controller: globalReplayController

    height: replaySettingGrid.implicitHeight + 2 * Style.mediumMargin
    width: replaySettingGrid.implicitWidth + 2 * Style.largeMargin

    color: Style.darkerGray

    GridLayout {
        id: replaySettingGrid
        anchors.centerIn: parent
        rows: 2
        flow: GridLayout.TopToBottom
        columnSpacing: Style.mediumMargin

        MyLabel {
            text: "Replay from folder"
        }
        MyTextEdit {
            width: 400
            Layout.leftMargin: Style.mediumMargin
            text: controller.loadPath
            onTextChanged: controller.loadPath = text
            enabled: !controller.isReplayStreaming
        }
        MyLabel {
            text: "Auto play"
        }
        Row {
            Layout.leftMargin: Style.mediumMargin
            MyButton {
                id: startReplayButton
                text: "Start"
                backgroundColor: Style.darkGray
                enabled: controller.canReplayStream && !controller.isReplayStreaming && !controller.isCameraStreaming
                visible: !controller.isReplayStreaming
                onClicked: controller.startReplayStream()
            }
            MyButton {
                text: "Stop"
                backgroundColor: Style.darkGray
                visible: controller.isReplayStreaming
                onClicked: controller.stopReplayStream()
                width: startReplayButton.width
            }
        }
        MyLabel {
            text: "Fps"
        }
        MyTextEdit {
            width: 60
            Layout.leftMargin: Style.mediumMargin
            text: controller.replayFps
            onTextChanged: controller.replayFps = text
            enabled: !controller.isReplayStreaming
        }
    }
}
