import CppObjects 1.0
import MpsStyle 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15

// Компонент - строка для настройки частоты сигнала
Item {
    id: control

    readonly property real marginValue: Style.px(3)

    implicitHeight: Style.px(10)

    Connections {
        function onFrequencyChanged() {
            hzInput.text = CppObjects.signalParameters.frequency;
            slider.value = CppObjects.signalParameters.frequency;
        }

        target: CppObjects.signalParameters
    }

    Slider {
        id: slider

        property bool isInit: false

        stepSize: 1
        from: CppObjects.signalParameters.minFrequency
        to: CppObjects.signalParameters.maxFrequency
        onValueChanged: {
            if (slider.isInit)
                CppObjects.signalParameters.frequency = slider.value;

        }
        Component.onCompleted: {
            slider.value = CppObjects.signalParameters.frequency;
            slider.isInit = true;
        }

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: control.marginValue
            right: valueItem.left
            rightMargin: control.marginValue
        }

    }

    Rectangle {
        id: valueItem

        readonly property real marginValue: Style.px(1)

        width: hzText.implicitWidth + hzInput.width + valueItem.marginValue * 2.5
        height: control.implicitHeight * 0.8
        color: Style.lineColor

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: control.marginValue
        }

        TextInput {
            id: hzInput

            function setFrequency() {
                // Если вдруг в TextInput появилось не число, то при отладке сообщаем разработчику

                const num = +hzInput.text;
                if (!isNaN(num))
                    CppObjects.signalParameters.frequency = num;
                else
                    console.assert(false, "SliderItem: Error in converting string to num");
            }

            width: Style.px(5.3)
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: TextInput.AlignRight
            font.pixelSize: Style.px(3)
            color: Style.textColorWhite
            inputMethodHints: Qt.ImhDigitsOnly
            onActiveFocusChanged: {
                if (!hzInput.activeFocus)
                    hzInput.setFrequency();

            }
            Keys.onReturnPressed: {
                hzInput.setFrequency();
            }
            Component.onCompleted: {
                hzInput.text = CppObjects.signalParameters.frequency;
            }

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: valueItem.marginValue
            }

            validator: IntValidator {
                top: slider.to
                bottom: slider.from
            }

        }

        Text {
            id: hzText

            text: qsTr("Гц")
            color: Style.textColorWhite
            font.pixelSize: Style.px(3)

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: valueItem.marginValue
            }

        }

    }

}
