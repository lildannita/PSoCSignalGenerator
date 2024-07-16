import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15

// Компонент - строка для настройки коэф. заполненности сигнала
Item {
    id: control

    readonly property real marginValue: Style.px(3)
    readonly property real buttonWidth: (control.width - control.marginValue * 3) / 2
    readonly property real buttonHeight: control.implicitHeight * 0.8

    implicitHeight: Style.px(10)

    ButtonItem {
        height: control.buttonHeight
        width: control.buttonWidth
        textStr: CppObjects.signalParameters.smallDutyCycle + "%"
        isChecked: CppObjects.signalParameters.dutyCycle === CppObjects.signalParameters.smallDutyCycle
        onItemClicked: {
            CppObjects.signalParameters.dutyCycle = CppObjects.signalParameters.smallDutyCycle;
        }

        anchors {
            left: parent.left
            leftMargin: control.marginValue
            verticalCenter: control.verticalCenter
        }

    }

    ButtonItem {
        height: control.buttonHeight
        width: control.buttonWidth
        textStr: CppObjects.signalParameters.bigDutyCycle + "%"
        isChecked: CppObjects.signalParameters.dutyCycle === CppObjects.signalParameters.bigDutyCycle
        onItemClicked: {
            CppObjects.signalParameters.dutyCycle = CppObjects.signalParameters.bigDutyCycle;
        }

        anchors {
            right: parent.right
            rightMargin: control.marginValue
            verticalCenter: control.verticalCenter
        }

    }

}
