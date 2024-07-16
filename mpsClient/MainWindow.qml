import ConnectionPage 1.0
import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import SettingsPage 1.0

ApplicationWindow {
    id: mainWin

    Component.onCompleted: {
        // Устновка разрешения экрана
        console.info("Screen.devicePixelRatio =", Screen.devicePixelRatio);
        Style.screenPixelDensity = Qt.binding(function() {
            return Screen.pixelDensity;
        });
    }
    width: CppObjects.appState.isAndroid ? Screen.width : Screen.width / 2.5
    height: CppObjects.appState.isAndroid ? Screen.height : Screen.height / 1.2
    visible: true
    title: qsTr("Генератор сигналов")
    // При закрытии приложения закрываем соединение и сообщаем об этом устройству
    onClosing: {
        CppObjects.bluetoothHandler.disconnectDevice();
    }

    // Связываем сигнал, сообщающий о наличии ошибки, и отображение диалога с ошибкой
    Connections {
        // Отображение ошибки в GUI
        function onShowError(msg) {
            // Создаем объект - диалог об ошибке, и открываем его
            const errorPopupComponent = Qt.createComponent("qrc:/MpsStyle/ErrorPopup.qml");
            const errorPopup = errorPopupComponent.createObject(mainWin, {
                "parent": mainWin.contentItem,
                "errText": msg
            }) as ErrorPopup;
            errorPopup.open();
            // Запрещаем переключение на другую страницу
            footerBar.enabled = false;
            // Обработчик закрытия диалога
            errorPopup.onClosed.connect(function() {
                // Разрешаем переключение на другую страницу
                footerBar.enabled = true;
            });
        }

        // Цель - обработчик Bluetooth соединения
        target: CppObjects.bluetoothHandler
    }

    // Расстановка основных вкладок приложения
    StackLayout {
        id: content

        width: parent.width
        height: mainWin.contentItem.height
        currentIndex: footerBar.currentIndex

        // Страница с подключением и настройкой параметров сигналов
        ConnectionTab {
        }

        // Страница с настройкой приложения и установкой пароля
        SettingsTab {
        }

    }

    // Подвал приложения, управляющий вкладками
    footer: MpsFooter {
        id: footerBar

        width: parent.width
        height: Style.px(10)
        currentIndex: 0

        MpsFooterItem {
            text: qsTr("Соединение")
            icon.source: "qrc:/bluetooth.svg"
        }

        MpsFooterItem {
            text: qsTr("Параметры")
            icon.source: "qrc:/settings.svg"
        }

    }

}
