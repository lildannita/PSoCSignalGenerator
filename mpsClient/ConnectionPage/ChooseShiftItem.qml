import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15

// Компонент - строка для настройки сдвига сигнала
Item {
    id: control

    readonly property real marginValue: Style.px(3)
    readonly property real buttonWidth: (control.width - control.marginValue * 3) / 2
    readonly property real buttonHeight: control.implicitHeight * 0.8

    implicitHeight: Style.px(10)

    ButtonItem {
        height: control.buttonHeight
        width: control.buttonWidth
        textStr: CppObjects.signalParameters.smallShift / 1000 + qsTr(" мс")
        isChecked: CppObjects.signalParameters.shift === CppObjects.signalParameters.smallShift
        onItemClicked: {
            CppObjects.signalParameters.shift = CppObjects.signalParameters.smallShift;
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
        textStr: CppObjects.signalParameters.bigShift / 1000 + qsTr(" мс")
        isChecked: CppObjects.signalParameters.shift === CppObjects.signalParameters.bigShift
        onItemClicked: {
            CppObjects.signalParameters.shift = CppObjects.signalParameters.bigShift;
        }

        anchors {
            right: parent.right
            rightMargin: control.marginValue
            verticalCenter: control.verticalCenter
        }

    }

}
