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

    height: recordSettingGrid.implicitHeight + 2 * Style.mediumMargin
    width: recordSettingGrid.implicitWidth + 2 * Style.largeMargin

    color: Style.darkerGray

    GridLayout {
        id: recordSettingGrid
        anchors.centerIn: parent
        rows: 2
        flow: GridLayout.TopToBottom
        columnSpacing: Style.mediumMargin

        MyCheckBox {
            id: saveToDiskCheckBox
            text: "Record to folder"
            checked: controller.saveallframesEnabled
            onCheckedChanged: controller.saveallframesEnabled = checked
            Layout.fillWidth: true

            MyButton {
                anchors.right: saveToDiskCheckBox.right
                anchors.bottom: saveToDiskCheckBox.bottom
                backgroundColor: Style.darkGray
                text: "Save single frame"
                enabled: !controller.saveallframesEnabled
                onClicked: controller.saveSingleFrame()
            }
        }
        MyTextEdit {
            id: saveToDiskPathEdit
            Layout.leftMargin: Style.mediumMargin
            enabled: !saveToDiskCheckBox.checked
            width: 400
            text: controller.savePath
            onTextChanged: controller.savePath = text
        }
        MyLabel {
            text: "Skip frames"
        }
        MyTextEdit {
            Layout.leftMargin: Style.mediumMargin
            width: 60
            text: controller.skipSavingFrames
            onTextChanged: controller.skipSavingFrames = parseInt(text)
        }
    }
}
