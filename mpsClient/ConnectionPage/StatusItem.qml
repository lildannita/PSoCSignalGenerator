import MpsStyle 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

// Компонент - строка, в которой отображаем состояние подключения к устройству
Item {
    id: connectItem

    property alias textStr: text.text
    property bool isOk: false
    property bool isLoading: false

    height: Style.px(5)
    implicitWidth: text.implicitWidth + icon.sourceSize.width + Style.px(3)

    Text {
        id: text

        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Style.px(3)
        color: Style.textColorBlack

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

    }

    IconImage {
        visible: !connectItem.isLoading
        source: connectItem.isOk ? "qrc:/ok.svg" : "qrc:/error.svg"
        sourceSize.width: parent.height
        sourceSize.height: parent.height

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

    }

    IconImage {
        id: icon

        visible: connectItem.isLoading
        source: "qrc:/loading.svg"
        sourceSize.width: parent.height
        sourceSize.height: parent.height

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        NumberAnimation on rotation {
            id: animation

            from: 0
            to: 360
            duration: 2000
            loops: Animation.Infinite
        }

    }

}
