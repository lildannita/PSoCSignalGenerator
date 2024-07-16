import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T
import QtQuick.Window 2.15

// Компонент - кнопка в "подвале"
T.TabButton {
    id: control

    implicitWidth: Math.max(control.background ? control.background.implicitWidth : 0, control.contentItem.implicitWidth)
    implicitHeight: parent.height
    padding: 0
    spacing: 0

    contentItem: Item {
        IconImage {
            id: icon

            anchors.top: parent.top
            anchors.topMargin: Style.px(0.5)
            anchors.horizontalCenter: parent.horizontalCenter
            source: control.icon.source
            sourceSize.width: Style.px(4.4)
            sourceSize.height: Style.px(4.4)
            color: Style.textColorWhite
        }

        Text {
            id: text

            anchors.top: icon.bottom
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Style.px(3)
            text: control.text
            color: Style.textColorWhite
        }

    }

    background: Rectangle {
        height: parent.height
        color: control.checked ? Style.backgroundSelectedFooterTabItem : Style.backgroundFooterTab
        radius: 3
    }

}
