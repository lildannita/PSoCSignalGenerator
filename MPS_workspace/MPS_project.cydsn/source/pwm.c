#include "pwm.h"

#include "global.h"

// Ссылки на глобальные переменные
volatile uint8_t _changedParamsState;
struct PwmParametrs _pwmParametrs;

// Частота Clock
#define PWM_CLOCK_FREQUENCY 1e6

// Объявление "приватных" функций
void setPwmParameters();
void startPwmWithShift();
void stopAndClearPwm();

// Инициализация генераторов ШИМ сигналов
void initPwm() {
    // Запуск со сдвигом генераторов ШИМ сигналов
    startPwmWithShift();
    // Установка исходных параметров ШИМ сигналов
    setPwmParameters();
}

// Запуск генераторов ШИМ сигналов со сдвигом
void startPwmWithShift() {
    PWM_1_Start();
    CyDelayUs(_pwmParametrs.shift);
    PWM_2_Start();
    CyDelayUs(_pwmParametrs.shift);
    PWM_3_Start();
    CyDelayUs(_pwmParametrs.shift);
    PWM_4_Start();
}

// Остановка генераторов ШИМ сигналов и очистка счетчиков
void stopAndClearPwm() {
    // Остановка генераторов ШИМ сигналов
    PWM_1_Stop();
    PWM_2_Stop();
    PWM_3_Stop();
    PWM_4_Stop();
    // Очистка счетчиков генераторов ШИМ сигналов
    PWM_1_WriteCounter(0);
    PWM_2_WriteCounter(0);
    PWM_3_WriteCounter(0);
    PWM_4_WriteCounter(0);
}

// Установка текущих параметров ШИМ сигналов
void setPwmParameters() {
    // Расчет периода для генераторов ШИМ сигналов
    uint16_t period = PWM_CLOCK_FREQUENCY / _pwmParametrs.frequency;
    // Расчет сравнения для генераторов ШИМ сигналов
    uint16_t compare = period * _pwmParametrs.dutyCycle / 100;

    // Установка рассчитанных значений для генераторов ШИМ сигналов
    PWM_1_WritePeriod(period);
    PWM_1_WriteCompare(compare);
    PWM_2_WritePeriod(period);
    PWM_2_WriteCompare(compare);
    PWM_3_WritePeriod(period);
    PWM_3_WriteCompare(compare);
    PWM_4_WritePeriod(period);
    PWM_4_WriteCompare(compare);
}

// Обработка изменений параметров ШИМ сигналов
void handlePwmParametersChange() {
    // Если изменили частоту или коэффициент заполнения,
    // то нужно только установить текущие значения для генераторов
    if ((_changedParamsState & (1 << MODE_UPDATE_FREQUENCY)) ||
        (_changedParamsState & (1 << MODE_UPDATE_DUTY_CYCLE))) {
        // Установка текущих параметров ШИМ сигналов
        setPwmParameters();
        // Обработка изменения частоты или коэф. заполнения закончилась
        _changedParamsState &= ~(1 << MODE_UPDATE_FREQUENCY);
        _changedParamsState &= ~(1 << MODE_UPDATE_DUTY_CYCLE);
    }

    // Если изменили величину сдвига, то нужно перезапустить
    // генераторы с текущим сдвигом
    if (_changedParamsState & (1 << MODE_UPDATE_SHIFT)) {
        // Перезапуск генераторов с текущим сдвигом
        stopAndClearPwm();
        startPwmWithShift();
        // Обработка изменения величины сдвига закончилась
        _changedParamsState &= ~(1 << MODE_UPDATE_SHIFT);
    }
}
