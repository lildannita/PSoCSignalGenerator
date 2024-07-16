import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15
import QtQuick.Layouts 1.15

// Страница с настройкой приложения
MpsPage {
    ColumnLayout {
        spacing: 0

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        MpsHeaderItem {
            Layout.fillWidth: true
            textStr: qsTr("Пароль для подключения к устройству")
            hasTopLine: false
        }

        PasswordItem {
            Layout.fillWidth: true
        }

        MpsHeaderItem {
            Layout.fillWidth: true
            textStr: qsTr("Настройки")
        }

        SwitchLanguageItem {
            Layout.fillWidth: true
        }

    }

}
