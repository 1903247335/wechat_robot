import QtQuick 2.15
import QtQuick.Controls

Button {
    id: control
    text: btnText

    property string btnText: "输入文字"
    property color btnColor: "#2196F3"

    background: Rectangle {
        implicitWidth: 80
        implicitHeight: 30
        color: control.pressed ? Qt.darker(btnColor, 1.2)
                               : (control.hovered ? Qt.lighter(btnColor, 1.1) : btnColor)
        radius: 4
    }

    contentItem: Text {
        text: control.text
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
