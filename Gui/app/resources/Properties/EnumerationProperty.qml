import QtQuick 2.0
import QtQuick.Controls 2.2
import Dnai.Controls 1.0
import Dnai.FontAwesome 1.0
import DNAI.Core 1.0
import Dnai.Theme 1.0

BaseProperty {
    id: _panel
    property var listmodel: null
    property real contentHeight: 24
    property real valueSpacing: 5
    property Button createButton: _createButton
    property int entityID: -1

    anchors.left: parent.left
    anchors.right: parent.right
    height: _createButton.height + _values.height + _panel.content.anchors.topMargin * 2 + header.height - _panel.valueSpacing + _panel.spacing * 2

    Item {
        id: _values
        anchors.left: parent.left
        anchors.right: parent.right
        height: (_panel.contentHeight + _panel.valueSpacing) * _panel.model[prop].length

        ListView {
            anchors.fill: parent
            spacing: _panel.valueSpacing
            model: _panel.model[prop]
            onModelChanged: {
                _panel.height = _panel.height = _createButton.height + (_panel.contentHeight + _panel.valueSpacing) * _panel.model[prop].length + _panel.content.anchors.topMargin * 2 + _panel.header.height - _panel.valueSpacing + _panel.spacing * 2
            }

            delegate: EnumValue {
                value: modelData
                prop: _panel.prop
                height: _panel.contentHeight
                anchors.left: parent.left
                anchors.right: parent.right
                contentHeight: _panel.contentHeight

                onMovedUp: {
                    if (_panel.model !== null)
                    {
                        _panel.model.moveUp(index)
                    }
                }

                onMovedDown: {
                    if (_panel.model !== null)
                    {
                        _panel.model.moveDown(index)
                    }
                }

                onRemoved: {
                    if (_panel.model !== null)
                    {
                        Controller.enumeration.removeValue(entityID, value)
                    }
                }

                onValueUpdated: {
                    if (_panel.model !== null)
                    {
                        Controller.enumeration.renameValue(entityID, oldValue, newValue);
                    }
                }
            }
        }
    }
    Button {
        id: _createButton
        awesomeIcon.text: "\uf055"
        anchors.horizontalCenter: parent.horizontalCenter
        awesomeIcon.size: 30
        height: 40
        width: 40
        awesomeIcon.color: "#ffffff"
        background: Rectangle {
            color: "transparent"
        }
        onClicked: {
            if (_panel.model !== null)
            {
                Controller.enumeration.setValue(entityID, "", Math.floor(Math.random() * 1000));
                //_panel.model.addEntry("");
                _panel.height = _createButton.height + (_panel.contentHeight + _panel.valueSpacing) * _panel.model[prop].length + _panel.content.anchors.topMargin * 2 + header.height - _panel.valueSpacing + _panel.spacing * 2
            }
        }
    }
}
