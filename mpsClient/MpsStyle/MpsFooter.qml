import QtQuick 2.15
import QtQuick.Templates 2.15 as T

// Компонент - "подвал" приложения
T.TabBar {
    id: tabBar

    spacing: 1
    padding: 1

    contentItem: ListView {
        model: tabBar.contentModel
        currentIndex: tabBar.currentIndex
        spacing: tabBar.spacing
        orientation: ListView.Horizontal
        flickableDirection: Flickable.AutoFlickIfNeeded
        boundsBehavior: Flickable.StopAtBounds
    }

    background: Rectangle {
        id: backgroundRect

        color: Style.backgroundFooterTab
    }

}
