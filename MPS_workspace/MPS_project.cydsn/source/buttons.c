#include "buttons.h"

#include "bluetooth.h"
#include "global.h"
#include "lcd.h"
#include "signals.h"

// Ссылки на глобальные переменные
volatile uint8_t _state;
volatile uint8_t _changedParamsState;
volatile uint8_t _displayMenuPoint;
volatile bool _isSystemInit;

// Обработка нажатия кнопки "Назад"
CY_ISR(Back_Handler) {
    // Если устройство еще не настроено, не реагируем на прерывание
    if (!_isSystemInit) {
        return;
    }

    // Если находимся на странице меню, то некуда возвращаться
    if (_state & (1 << MODE_MENU)) {
        return;
    }
    // Если есть несохраненные изменения, то сохраняем перед выходом
    if (_changedParamsState != 0x00) {
        handleSignalParamsChange();
    }
    // Если находились на странице с Bluetooth управлением, то перед выходом
    // отключаем Bluetooth соединение
    if (_state & (1 << MODE_BLUETOOTH_CONNECTION)) {
        stopBluetoothConnection();
    }
    // Изменяем текущее состояние устройства
    _state &= ~(1 << _displayMenuPoint);
    _state |= 1 << MODE_MENU;
    // Отображаем страницу с меню
    printDisplayMenu();
}

// Обработка нажатия кнопки "ОК"
CY_ISR(Ok_Handler) {
    // Если устройство еще не настроено, не реагируем на прерывание
    if (!_isSystemInit) {
        return;
    }

    // Если находимся не на странице меню, то некуда переходить
    if (!(_state & (1 << MODE_MENU))) {
        // Если есть несохраненные изменения, то сохраняем
        if (_changedParamsState != 0x00) {
            handleSignalParamsChange();
        }
        return;
    }
    // Изменяем текущее состояние устройства
    _state &= ~(1 << MODE_MENU);
    _state |= 1 << _displayMenuPoint;
    // Открываем страницу в зависимости от того, где находится курсор
    switch (_displayMenuPoint) {
        case MODE_UPDATE_FREQUENCY: {
            printFrequencyPage(true);
            break;
        }
        case MODE_UPDATE_DUTY_CYCLE: {
            printDutyCyclePage(true);
            break;
        }
        case MODE_UPDATE_SHIFT: {
            printShiftPage(true);
            break;
        }
        case MODE_INFO: {
            printInfoPage(true);
            break;
        }
        case MODE_BLUETOOTH_CONNECTION: {
            startBluetoothConnection();
            break;
        }
        default:
            break;
    }
}

// Инициализация прерываний от кнопок
void initButtonsIsr() {
    // Установка приоритета нажатий
    Back_ISR_SetPriority(1);
    Ok_ISR_SetPriority(2);
    // Указание обработчиков прерывания
    Back_ISR_StartEx(Back_Handler);
    Ok_ISR_StartEx(Ok_Handler);
}
