#include "slider.h"

#include <math.h>

#include "global.h"
#include "lcd.h"

// Для конвертирования sliderPosition в диапазон 0x01 - 0x08
#define SLIDER_CONVERT 3
// Для настройки реакции на движения пользователя.
// Чем значение больше - тем пользователю нужно
// больше проводить пальцем для изменения значения.
#define SLIDER_DIFF 4
// Величина изменения частоты сигналов при обработке одного
// движения по слайдеру
#define FREQUENCY_DIFF 10

// Ссылки на глобальные переменные
volatile uint8_t _state;
volatile uint8_t _changedParamsState;
volatile uint8_t _displayMenuPoint;
uint8_t _displayMenuState;
struct PwmParametrs _pwmParametrs;

// Указатель на текущий номер строки LCD-дисплея
bool _displayMenuIsFirstRow = true;

// Объявление "приватных" функций
void updateDisplayMenuState(const SlideDirection direction);
void updateFrequencyValue(const SlideDirection direction);
void updateDutyCycleValue(const SlideDirection direction);
void handleScroll(const SlideDirection direction);

// Инициализация слайдера
void initSlider() {
    // Запуск CapSense модуля
    CapSense_Slider_Start();
    // Настройка модуля на слайдер
    CapSense_Slider_InitializeEnabledBaselines();
}

// Бесконечная проверка движения по слайдеру
void infinitiveSliderCheck() {
    // Текущая позиция на слайдере
    uint8_t sliderPosition = 0xFF;
    // Позиция на слайдере при первом касании
    uint8_t firstSliderIndex = 0xFF;
    // Разница между первым и текущим касаниями
    int8_t indexDifference = 0;
    for (;;) {
        // Сканирование доступных CapSense виджетов, т.е. слайдера
        CapSense_Slider_ScanEnabledWidgets();
        // Пока не закончилось сканирование
        while (CapSense_Slider_IsBusy() != 0)
            ;
        // Обновление состояния слайдера
        CapSense_Slider_UpdateEnabledBaselines();

        // Считываем текущее положение на слайдере
        sliderPosition = CapSense_Slider_GetCentroidPos(CapSense_Slider_LINEARSLIDER0__LS);
        if (sliderPosition != 0xFF) {
            // Если есть касание, то конвертируем число в порядковый номер
            sliderPosition = 1 << (sliderPosition >> SLIDER_CONVERT);
            if (firstSliderIndex == 0xFF) {
                // Если первое касание не сохранено, то записываем
                firstSliderIndex = sliderPosition;
            } else {
                // Разница между первым и текущим касаниями
                indexDifference = firstSliderIndex - sliderPosition;
                // Если разница больше установленной реакции на движение
                // то обрабатываем движение по слайдеру
                if (abs(indexDifference) >= SLIDER_DIFF) {
                    // Обработка движения по слайдеру
                    handleScroll(indexDifference > 0 ? SLIDE_LEFT : SLIDE_RIGHT);
                    // Так как обработали одно "движение" по слайдеру, то теперь
                    // позиция первого касания становится позицией последнего
                    firstSliderIndex = sliderPosition;
                }
            }
        } else {
            // "Обнуление" позиции при первом касании
            firstSliderIndex = 0xFF;
        }
    }
}

// Обновление отображаемых пунктов меню
void updateDisplayMenuState(const SlideDirection direction) {
    switch (direction) {
        // Движение вправо (вниз по меню)
        case SLIDE_RIGHT: {
            if (_displayMenuIsFirstRow) {
                // Если курсор показывал на первую строку дисплея,
                // то изменяем только положение курсора
                _displayMenuIsFirstRow = false;
            } else if (_displayMenuState & (1 << MENU_BOTTOM)) {
                // Если находимся в конце списка меню, то ничего не делаем
                return;
            } else {
                // Сдвигаем влево состояние отображаемых пунктов меню
                _displayMenuState = _displayMenuState << 1;
            }
            // Указатель на текущий пункт меню
            _displayMenuPoint += 1;
            break;
        }
        // Движение влево (вверх по меню)
        case SLIDE_LEFT: {
            if (!_displayMenuIsFirstRow) {
                // Если курсор показывал на последнюю строку дисплея,
                // то изменяем только положение курсора
                _displayMenuIsFirstRow = true;
            } else if (_displayMenuState & (1 << MENU_TOP)) {
                // Если находимся в начале списка меню, то ничего не делаем
                return;
            } else {
                // Сдвигаем вправо состояние отображаемых пунктов меню
                _displayMenuState = _displayMenuState >> 1;
            }
            // Указатель на текущий пункт меню
            _displayMenuPoint -= 1;
            break;
        }
        // Неизвестное движение
        default:
            // UNREACHABLE
            assert(false);
    }
    // Обновляем отображаемые пункты меню
    printDisplayMenu();
}

// Обновление значения частоты сигналов
void updateFrequencyValue(const SlideDirection direction) {
    switch (direction) {
        // Движение вправо (увеличение частоты)
        case SLIDE_RIGHT: {
            if (_pwmParametrs.frequency == MAX_FREQUENCY) {
                // Если достигли правой границы допустимого диапазона,
                // то ничего не делаем.
                return;
            }
            // Увеличиваем частоту на заданное значение
            _pwmParametrs.frequency += FREQUENCY_DIFF;
            break;
        }
        // Движение влево (уменьшение частоты)
        case SLIDE_LEFT: {
            if (_pwmParametrs.frequency == MIN_FREQUENCY) {
                // Если достигли левой границы допустимого диапазона,
                // то ничего не делаем.
                return;
            }
            // Уменьшаем частоту на заданное значение
            _pwmParametrs.frequency -= FREQUENCY_DIFF;
            break;
        }
        // Неизвестное движение
        default:
            // UNREACHABLE
            assert(false);
    }
    // Выводим обновленную частоту
    printFrequencyPage(false);
    // Показатель того, что значение частоты изменено,
    // но это изменение еще не принято
    _changedParamsState |= (1 << MODE_UPDATE_FREQUENCY);
}

// Обновление значения коэффициента заполнения (скважности) сигналов
void updateDutyCycleValue(const SlideDirection direction) {
    switch (direction) {
        // Движение вправо (коэф. заполнения = 50%)
        case SLIDE_RIGHT: {
            if (_pwmParametrs.dutyCycle == DUTY_CYCLE_HALF) {
                // Если коэф. заполнения уже = 50%, то ничего не делаем
                return;
            }
            // Изменяем значение коэф. заполнения
            _pwmParametrs.dutyCycle = DUTY_CYCLE_HALF;
            break;
        }
        // Движение влево (коэф. заполнения = 25%)
        case SLIDE_LEFT: {
            if (_pwmParametrs.dutyCycle == DUTY_CYCLE_QUARTER) {
                // Если коэф. заполнения уже = 25%, то ничего не делаем
                return;
            }
            // Изменяем значение коэф. заполнения
            _pwmParametrs.dutyCycle = DUTY_CYCLE_QUARTER;
            break;
        }
        // Неизвестное движение
        default:
            // UNREACHABLE
            assert(false);
    }
    // Выводим обновленный коэф. заполнения
    printDutyCyclePage(false);
    // Показатель того, что значение коэффициента заполнения изменено,
    // но это изменение еще не принято
    _changedParamsState |= (1 << MODE_UPDATE_DUTY_CYCLE);
}

// Обновление значения сдвига сигналов
void updateShiftValue(const SlideDirection direction) {
    switch (direction) {
        // Движение влево (сдвиг = 2мс)
        case SLIDE_RIGHT: {
            if (_pwmParametrs.shift == SHIFT_BIG) {
                // Если сдвиг уже = 2мс, то ничего не делаем
                return;
            }
            // Изменяем значение коэф. заполнения
            _pwmParametrs.shift = SHIFT_BIG;
            break;
        }
        // Движение влево (сдвиг = 0.5мс)
        case SLIDE_LEFT: {
            if (_pwmParametrs.shift == SHIFT_SMALL) {
                // Если сдвиг уже = 0.5мс, то ничего не делаем
                return;
            }
            // Изменяем значение сдвига
            _pwmParametrs.shift = SHIFT_SMALL;
            break;
        }
        // Неизвестное движение
        default:
            // UNREACHABLE
            assert(false);
    }
    // Выводим обновленный сдвиг
    printShiftPage(false);
    // Показатель того, что значение сдвига изменено,
    // но это изменение еще не принято
    _changedParamsState |= (1 << MODE_UPDATE_SHIFT);
}

// Обработка движения по слайдеру
void handleScroll(const SlideDirection direction) {
    if (_state & (1 << MODE_MENU)) {
        // Если находимся на странице меню
        updateDisplayMenuState(direction);
    } else if (_state & (1 << MODE_UPDATE_FREQUENCY)) {
        // Если находимся на странице изменения частоты сигналов
        updateFrequencyValue(direction);
    } else if (_state & (1 << MODE_UPDATE_DUTY_CYCLE)) {
        // Если находимся на странице изменения коэф. заполнения
        updateDutyCycleValue(direction);
    } else if (_state & (1 << MODE_UPDATE_SHIFT)) {
        // Если находимся на странице изменения сдвига
        updateShiftValue(direction);
    }
}
