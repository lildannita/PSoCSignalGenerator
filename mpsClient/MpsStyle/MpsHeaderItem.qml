import MpsStyle 1.0
import QtQuick 2.15

// Компонент - заголовок блока
Rectangle {
    id: control

    property alias textStr: text.text
    property bool hasTopLine: true
    property bool hasBottomLine: true

    implicitHeight: text.font.pixelSize * 1.5 + (topLine.visible ? topLine.height : 0) + (bottomLine.visible ? bottomLine.height : 0)
    color: Style.backgroundHeaderInfoItem

    Rectangle {
        id: topLine

        height: 2
        color: Style.lineColor
        visible: control.hasTopLine

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

    }

    Text {
        id: text

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: Style.px(3)
        color: Style.textColorBlack

        anchors {
            top: topLine.visible ? topLine.bottom : parent.top
            bottom: bottomLine.visible ? bottomLine.top : parent.bottom
            right: parent.right
            left: parent.left
            leftMargin: Style.px(3)
        }

    }

    Rectangle {
        id: bottomLine

        height: 2
        color: Style.lineColor
        visible: control.hasBottomLine

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

    }

    // При нажатии на заголовок убираем фокус с любого компонента
    MouseArea {
        anchors.fill: parent
        onClicked: {
            control.forceActiveFocus();
        }
    }

}
