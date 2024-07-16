#include "signals.h"

#include "global.h"
#include "pwm.h"
#include "vdacSin.h"

// Инициализация генераторов сигналов
void initSignalGenerators() {
	// Установка исходных параметров сигналов
    _pwmParametrs.frequency = MAX_FREQUENCY;
    _pwmParametrs.dutyCycle = DUTY_CYCLE_HALF;
    _pwmParametrs.shift = SHIFT_SMALL;

    // Инициализация генератора ШИМ-сигналов
    initPwm();
    // Инициаилизация генератора синусоидальных сигналов
    initVdacSin();
}

// Установка новых параметров для генераторов
void handleSignalParamsChange() {
    // Установка новых параметров для генератора ШИМ-сигналов
    handlePwmParametersChange();
    // Установка новых параметров генератора синусоидальных сигналов
    handleSinSignalFrequencyChange();
    // Все изменения параметров применены
    _changedParamsState = 0;
}