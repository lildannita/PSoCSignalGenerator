import MpsStyle 1.0
import QtQuick 2.15

// Комонент - кнопка для выбора параметров сигналов
Rectangle {
    id: control

    property alias textStr: text.text
    property bool isChecked: false

    signal itemClicked()

    implicitHeight: text.implicitHeight
    implicitWidth: text.implicitWidth
    color: control.isChecked ? Style.backgroundCheckedItem : Style.backgroundUncheckedItem
    radius: Style.px(1)

    Text {
        id: text

        anchors.centerIn: parent
        color: Style.textColorWhite
        font.pixelSize: Style.px(3)
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            control.forceActiveFocus();
            control.itemClicked();
        }
    }

}
