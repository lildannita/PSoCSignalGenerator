import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15

// Элемент настройки языка приложения
Item {
    id: control

    property bool hasBottomLine: true
    readonly property real marginValue: Style.px(3)

    implicitHeight: Style.px(10) + (control.hasBottomLine ? bottomLine.height : 0)

    Item {
        id: contentRow

        anchors {
            top: parent.top
            bottom: control.hasBottomLine ? bottomLine.top : parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: control.marginValue
            rightMargin: control.marginValue
        }

        Text {
            id: settingText

            verticalAlignment: Text.AlignVCenter
            text: qsTr("Язык приложения")
            wrapMode: Text.Wrap
            elide: Text.ElideNone
            font.pixelSize: Style.px(3)
            color: Style.textColorBlack

            anchors {
                left: parent.left
                right: btnItem.left
                top: parent.top
                bottom: parent.bottom
                rightMargin: control.marginValue
            }

        }

        Rectangle {
            id: btnItem

            width: Math.max(langText.implicitWidth + Style.px(2), Style.px(10))
            height: Style.px(6)
            color: "transparent"
            radius: Style.px(3)
            border.color: Style.lineColor

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            Text {
                id: langText

                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: CppObjects.appState.isRussianLanguage ? "RUS" : "ENG"
                wrapMode: Text.NoWrap
                elide: Text.ElideNone
                font.pixelSize: Style.px(3)
                color: Style.textColorBlack
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    // При нажатии на кнопку - меняем язык приложения
                    CppObjects.appState.setIsRussianLanguage(!CppObjects.appState.isRussianLanguage);
                }
            }

        }

    }

    Rectangle {
        id: bottomLine

        height: 2
        color: Style.lineColor
        visible: control.hasBottomLine

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

    }

}
