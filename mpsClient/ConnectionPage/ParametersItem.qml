import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15
import QtQuick.Layouts 1.15

// Компонент - подстраница для настройки параметров
Item {
    ColumnLayout {
        id: layout

        spacing: 0

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        MpsHeaderItem {
            Layout.fillWidth: true
            textStr: qsTr("Частота")
            hasTopLine: false
        }

        SliderItem {
            Layout.fillWidth: true
        }

        MpsHeaderItem {
            Layout.fillWidth: true
            textStr: qsTr("Коэффициент заполнения")
        }

        ChooseDataCycleItem {
            Layout.fillWidth: true
        }

        MpsHeaderItem {
            Layout.fillWidth: true
            textStr: qsTr("Сдвиг")
        }

        ChooseShiftItem {
            Layout.fillWidth: true
        }

    }

    BluetoothButtonItem {
        id: buttonItem

        width: parent.width * 0.5
        buttonStr: qsTr("Отправить")
        onButtonClicked: {
            CppObjects.bluetoothHandler.sendSignalParameters();
        }

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: layout.bottom
            topMargin: Style.px(5)
        }

    }

    BluetoothButtonItem {
        id: buttonClosingItem

        width: parent.width * 0.5
        buttonStr: qsTr("Отключиться")
        btnColor: Style.btnHoveredErrorPopup
        btnPressedColor: Style.btnPressedErrorPopup
        btnTextColor: Style.textColorWhite
        onButtonClicked: {
            CppObjects.bluetoothHandler.disconnectDevice();
        }

        anchors {
            bottom: parent.bottom
            bottomMargin: Style.px(5)
            horizontalCenter: parent.horizontalCenter
        }

    }

}
