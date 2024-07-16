import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

// Элемент ввода пароля
Item {
    id: control

    readonly property real marginValue: Style.px(3)
    property bool isPasswordShown: false

    implicitHeight: Style.px(10)

    Text {
        id: text

        text: qsTr("Пароль")
        font.pixelSize: Style.px(3)
        color: Style.textColorBlack

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: control.marginValue
        }

    }

    TextInput {
        id: passwordInput

        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignRight
        echoMode: control.isPasswordShown ? TextInput.Normal : TextInput.Password
        font.pixelSize: Style.px(3)
        color: Style.textColorBlack
        clip: true
        onTextChanged: {
            // При изменении пароля сразу устанавливаем его в C++ и сохраняем в конфиг
            CppObjects.appState.setPassword(passwordInput.text);
        }
        onActiveFocusChanged: {
            if (passwordInput.activeFocus)
                passwordInput.selectAll();

        }
        Component.onCompleted: {
            passwordInput.text = CppObjects.appState.password;
        }

        anchors {
            verticalCenter: parent.verticalCenter
            right: icon.left
            rightMargin: control.marginValue
            left: text.right
            leftMargin: control.marginValue
        }

        Text {
            id: placeholderText

            anchors.fill: parent
            visible: !passwordInput.text && !passwordInput.activeFocus
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: TextInput.AlignRight
            font.pixelSize: Style.px(3)
            text: qsTr("Введите пароль")
            color: "gray"
        }

    }

    // Этот компонент управляет видимостью введенного пароля
    IconImage {
        id: icon

        source: control.isPasswordShown ? "qrc:/eye_black.svg" : "qrc:/eye.svg"
        sourceSize.width: Style.px(5)
        sourceSize.height: Style.px(5)

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: control.marginValue
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                // При нажатии меняем видимость
                control.isPasswordShown = !control.isPasswordShown;
            }
        }

    }

    // При нажатии на пустое место этого компонента переводим фокус на ввод пароля
    MouseArea {
        z: -1
        anchors.fill: parent
        onClicked: {
            passwordInput.forceActiveFocus();
        }
    }

}
