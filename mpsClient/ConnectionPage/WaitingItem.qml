import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

// Компонент - подстраница для подключения к устройству
Item {
    id: control

    readonly property real marginValue: Style.px(5)
    readonly property real statusWidth: Math.max(connectItem.implicitWidth, enabledItem.implicitWidth, control.width * 0.5)

    height: icon.height + control.marginValue * 2.5 + buttonItem.height + connectItem.height + enabledItem.height

    IconImage {
        id: icon

        source: "qrc:/bluetooth_error.svg"
        sourceSize.width: control.width * 0.7
        sourceSize.height: control.width * 0.7

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
        }

    }

    BluetoothButtonItem {
        id: buttonItem

        width: control.width * 0.5
        buttonStr: qsTr("Подключиться")
        onButtonClicked: {
            CppObjects.bluetoothHandler.startBluetoothConnection();
        }

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: icon.bottom
            topMargin: control.marginValue
        }

    }

    StatusItem {
        id: connectItem

        width: control.statusWidth
        isOk: CppObjects.bluetoothHandler.connected
        isLoading: CppObjects.bluetoothHandler.loadingConnected
        textStr: qsTr("Подключение к HC-05")

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: buttonItem.bottom
            topMargin: control.marginValue
        }

    }

    StatusItem {
        id: enabledItem

        width: control.statusWidth
        isOk: CppObjects.bluetoothHandler.controlEnabled
        isLoading: CppObjects.bluetoothHandler.loadingControlEnabled
        textStr: qsTr("Разрешение управления")

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: connectItem.bottom
            topMargin: control.marginValue * 0.5
        }

    }

}
