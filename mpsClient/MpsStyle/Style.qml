import QtQml 2.15
import QtQuick 2.15
pragma Singleton

// Компонент - контейнер стилевых настроек приложения
QtObject {
    id: valStyle

    property real screenPixelDensity: 1
    readonly property color textColorWhite: "#FFFFFF"
    readonly property color textColorBlack: "#000000"
    readonly property color backgroundPage: "#DCC9B6"
    readonly property color backgroundFooterTab: "#6D4C3D"
    readonly property color backgroundSelectedFooterTabItem: "#727D71"
    readonly property color backgroundHeaderInfoItem: "#ABC4AB"
    readonly property color lineColor: "#A39171"
    readonly property color backgroundCheckedItem: "#727D71"
    readonly property color backgroundUncheckedItem: "#6D4C3D"
    readonly property color backgroundButton: "#ABC4AB"
    readonly property color backgroundPressedButton: "#6D4C3D"
    readonly property color backgroundHeaderErrorPopup: "#B83C28"
    readonly property color btnPressedErrorPopup: "#9E3323"
    readonly property color btnHoveredErrorPopup: "#C4402B"
    readonly property color borderErrorPopup: "#B83C28"
    readonly property color backgroundErrorPopup: "#852B1D"

    // Функция пересчета миллиметров в пиксели, с учетом PDI
    function px(mm) {
        return valStyle.screenPixelDensity * mm;
    }

}
