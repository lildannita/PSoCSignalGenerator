import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15

// Компонент - страница для подключения и настройки сигналов
MpsPage {
    id: connectionTab

    // Булева переменная, отвечающая за разрешение настройки параметров
    readonly property bool isParametersShown: CppObjects.bluetoothHandler.connected && CppObjects.bluetoothHandler.controlEnabled

    ParametersItem {
        // Если можем управлять устройством, то показываем эту страницу
        anchors.fill: parent
        visible: connectionTab.isParametersShown
    }

    WaitingItem {
        // Если еще не можем управлять устройством, то показываем эту страницу
        visible: !connectionTab.isParametersShown

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }

    }

}
