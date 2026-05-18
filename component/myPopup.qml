import QtQuick
import QtQuick.Controls
import QtQuick.Effects // 必须导入，用于处理圆角阴影

Popup {
    id: control

    property alias text: messageText.text

    width: 320
    height: 150
    padding: 0
    anchors.centerIn: Overlay.overlay
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    Overlay.modal: Rectangle {
        color: "#66000000"
        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    enter: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 250; easing.type: Easing.OutCubic }
            NumberAnimation { property: "scale"; from: 0.8; to: 1.0; duration: 250; easing.type: Easing.OutBack }
        }
    }

    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150 }
    }

    background: Rectangle {
        radius: 16
        color: "#FFFFFF"
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#30000000"
            shadowBlur: 1.0
            shadowVerticalOffset: 8
            shadowHorizontalOffset: 0
        }
    }

    contentItem: Item {
        Column {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 20

            Text {
                id: messageText
                width: parent.width
                text: "默认文本"
                font.pixelSize: 16
                font.family: "Microsoft YaHei"
                color: "#2C3E50"
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                lineHeight: 1.2
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "我知道了"

                contentItem: Text {
                    text: parent.text
                    font.bold: true
                    color: parent.down ? "#BDC3C7" : "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    implicitWidth: 120
                    implicitHeight: 40
                    color: parent.down ? "#2980B9" : "#3498DB"
                    radius: 20
                }

                onClicked: control.close()
            }
        }
    }
}
