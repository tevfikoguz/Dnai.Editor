import QtQuick 2.0

import "../Style"

import DNAI 1.0
import Dnai.Settings 1.0
import Dnai.Theme 1.0

Item {
    id: buttonItemId

    signal pressed()
    signal hoverEnter()
    signal hoverExit()
    property string textValue: ""
    property int textPointSize: 15
    property alias textWidth: buttonLaunchText.width
    property alias textHeight: buttonLaunchText.height
    property string hoverColor: AppSettings.theme["button"]["text"]["hovercolor"]
    property string notHoverColor: AppSettings.theme["button"]["text"]["color"]
    property string colorRect: AppSettings.theme["button"]["color"]
    property string hoverColorRect: AppSettings.theme["button"]["hovercolor"]


    Rectangle {
        id: buttonLaunch
        anchors.fill: parent
        color: buttonItemId.colorRect

        Label {

            id: buttonLaunchText
            text: buttonItemId.textValue
            color: notHoverColor
            font.pointSize: buttonItemId.textPointSize
            anchors.horizontalCenter: buttonLaunch.horizontalCenter
            anchors.verticalCenter: buttonLaunch.verticalCenter

            function enter() {
                buttonLaunch.color = buttonItemId.hoverColorRect
                buttonLaunchText.color = hoverColor
                buttonItemId.hoverEnter()
            }

            function exit() {
                buttonLaunch.color = buttonItemId.colorRect
               buttonLaunchText.color = notHoverColor
                buttonItemId.hoverExit()
            }
        }

        MouseArea {
            id: buttonLaunchMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                buttonItemId.pressed()
            }
            onEntered: {
                buttonLaunchMouse.cursorShape = Qt.PointingHandCursor
                buttonLaunchText.enter()
            }
            onExited: {
                buttonLaunchMouse.cursorShape = Qt.ArrowCursor
                buttonLaunchText.exit()
            }
        }
    }

}
