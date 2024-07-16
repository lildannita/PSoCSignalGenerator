#include <device.h>

// Заголовочные файлы нужных модулей
#include "source/headers.h"

// Ссылки на глобальную переменную
volatile uint8_t _state = 0x00;
volatile bool _isSystemInit = false;

int main() {
    // Глобальное разрешение прерываний
    CyGlobalIntEnable;

    // Инициализация всех компонентов устройства
    initBluetoothConnection();
    initButtonsIsr();
    initSlider();
    initSignalGenerators();
    initLcdDisplay();

    // Исходное состояние устройства
    _state |= (1 << MODE_MENU);

    CyDelay(1000);
    // Устройство настроено на работу
    _isSystemInit = true;

    // Вывод стартовой страницы на LCD дисплей
    printDisplayMenu();
    // Запуск бесконечной проверки движения по слайдеру
    infinitiveSliderCheck();

    return 0;
}
