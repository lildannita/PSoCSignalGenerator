#pragma once

#include "../device.h"

// Границы частоты ШИМ сигналов
#define MIN_FREQUENCY 200
#define MAX_FREQUENCY 450
// Коэф. заполнения ШИМ-сигналов (== 1 / скважность)
#define DUTY_CYCLE_QUARTER 25
#define DUTY_CYCLE_HALF 50
// Сдвиг ШИМ-сигналов
#define SHIFT_SMALL 500
#define SHIFT_BIG 2000

// Границы списка параметров в меню
#define MENU_TOP MODE_UPDATE_FREQUENCY
// #define MENU_BOTTOM MODE_OUTPUT
#define MENU_BOTTOM MODE_BLUETOOTH_CONNECTION

// Для удобства определяем тип bool
typedef enum { false, true } bool;

// Направления движения по слайдеру
typedef enum { SLIDE_RIGHT, SLIDE_LEFT } SlideDirection;

// Определение битов состояния
typedef enum {
    MODE_MENU,
    MODE_UPDATE_FREQUENCY,
    MODE_UPDATE_DUTY_CYCLE,
    MODE_UPDATE_SHIFT,
    MODE_INFO,
    MODE_BLUETOOTH_CONNECTION,
    BLUETOOTH_ENABLED
} States;

// Структура-контейнер для параметров сигналов
extern struct PwmParametrs {
    uint16_t frequency;
    uint8_t dutyCycle;
    uint16_t shift;
} _pwmParametrs;

// Текущее состояние устройства
extern volatile uint8_t _state;
// Контроль измененных параметров
extern volatile uint8_t _changedParamsState;

// Указатель на текущий параметр в меню
extern volatile uint8_t _displayMenuPoint;
// Текущее состояние отображаемых параметров в меню
extern uint8_t _displayMenuState;

// Показатель того, что устройство настроено
extern volatile bool _isSystemInit;
