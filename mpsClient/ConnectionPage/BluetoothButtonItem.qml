import MpsStyle 1.0
import QtQuick 2.15

// Комонент - кнопка для выполнения "Bluetooth-действий"
Rectangle {
    id: buttonRect

    property alias buttonStr: buttonText.text
    property color btnPressedColor: Style.backgroundPressedButton
    property color btnColor: Style.backgroundButton
    property color btnTextColor: Style.textColorBlack
    property color btnTextPressedColor: Style.textColorWhite

    signal buttonClicked()

    height: Style.px(8)
    color: mouseArea.pressed ? buttonRect.btnPressedColor : buttonRect.btnColor
    radius: Style.px(5)
    border.color: Style.lineColor
    border.width: Style.px(0.5)

    Text {
        id: buttonText

        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Style.px(4)
        color: mouseArea.pressed ? buttonRect.btnTextPressedColor : buttonRect.btnTextColor
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onClicked: {
            buttonRect.buttonClicked();
        }
    }

}
