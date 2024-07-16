import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T

// "Абстрактный" компонент - страница приложения
T.Page {
    id: page

    property ApplicationWindow applicationWindow

    enabled: page.applicationWindow ? !page.applicationWindow.popupIsOpened : true

    Rectangle {
        anchors.fill: parent
        z: 10
        visible: !page.enabled
        color: "#80000000"
    }

    background: Rectangle {
        color: Style.backgroundPage
    }

}
