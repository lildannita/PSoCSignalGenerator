import MpsStyle 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Компонент - диалог об ошибке
Popup {
    id: popup

    property ApplicationWindow appWindow
    required property string errText
    readonly property real marginValue: Style.px(1)
    readonly property int borderWidth: 3

    signal onBtnClicked()

    height: headerNameRect.height + button.height + infoLayout.height
    width: Math.max(parent.width * 0.6, Style.px(60))
    x: (parent.width - popup.width) / 2
    y: (parent.height - popup.height) / 2
    padding: 0
    clip: true

    Rectangle {
        id: headerNameRect

        height: headerName.implicitHeight + headerBottomLine.height + popup.marginValue * 2
        color: Style.backgroundHeaderErrorPopup
        radius: 5

        anchors {
            top: parent.top
            topMargin: popup.borderWidth
            left: parent.left
            right: parent.right
            leftMargin: popup.borderWidth
            rightMargin: popup.borderWidth
        }

        Text {
            id: headerName

            anchors.fill: parent
            font.pixelSize: Style.px(3)
            font.bold: true
            text: qsTr("Упс...")
            color: Style.textColorWhite
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Rectangle {
            height: headerNameRect.radius
            color: Style.backgroundHeaderErrorPopup

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

        }

    }

    Rectangle {
        id: headerBottomLine

        height: 2
        color: Style.textColorWhite

        anchors {
            left: parent.left
            leftMargin: popup.borderWidth
            right: parent.right
            rightMargin: popup.borderWidth
            top: headerNameRect.bottom
        }

    }

    ColumnLayout {
        id: infoLayout

        spacing: 0

        anchors {
            top: headerBottomLine.bottom
            right: parent.right
            left: parent.left
            leftMargin: popup.borderWidth
            rightMargin: popup.borderWidth
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: errMsg.implicitHeight
            Layout.topMargin: popup.marginValue * 2
            Layout.bottomMargin: popup.marginValue * 2

            Text {
                id: errMsg

                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                text: popup.errText
                font.pixelSize: Style.px(3)
                font.bold: true
                color: Style.textColorWhite
            }

        }

    }

    Button {
        id: button

        height: buttonText.implicitHeight + popup.marginValue * 3
        onClicked: popup.close()

        anchors {
            left: parent.left
            right: parent.right
            leftMargin: popup.borderWidth
            rightMargin: popup.borderWidth
            bottom: parent.bottom
        }

        contentItem: Text {
            id: buttonText

            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Style.px(3)
            font.bold: true
            text: qsTr("OK")
            color: Style.textColorWhite
        }

        background: Rectangle {
            radius: popup.borderWidth
            color: button.pressed ? Style.btnPressedErrorPopup : button.hovered ? Style.btnHoveredErrorPopup : Style.borderErrorPopup
        }

    }

    background: Rectangle {
        anchors.fill: parent
        color: Style.backgroundErrorPopup
        radius: 7
        border.color: Style.borderErrorPopup
        border.width: popup.borderWidth
    }

}
