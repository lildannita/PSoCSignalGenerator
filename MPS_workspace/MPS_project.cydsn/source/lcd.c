#include "lcd.h"

// Заголовочный файл с командами для управления
// LCD дисплея по каналу I2C
#include "LiquidCrystal_I2C.h"

// Адрес LCD дисплея
#define LCD_ADDRESS 0x3F
// Параметры LCD дисплея
#define LCD_CHARS_IN_LINE 16
#define LCD_LINES 2

// Ссылки на глобальные переменные
volatile uint8_t _state;
volatile uint8_t _changedParamsState;
volatile uint8_t _displayMenuPoint;
uint8_t _displayMenuState;
struct PwmParametrs _pwmParametrs;

// Инциализация I2C и LCD дисплея
void initLcdDisplay() {
    // Отображаемые строки в меню
    _displayMenuState |= (1 << MODE_UPDATE_FREQUENCY) | (1 << MODE_UPDATE_DUTY_CYCLE);
    _displayMenuPoint = MODE_UPDATE_FREQUENCY;

    // Включение I2C
    LCD_I2C_Start();
    // Инциализация LCD дисплея
    LiquidCrystal_I2C_init(LCD_ADDRESS, LCD_CHARS_IN_LINE, LCD_LINES, 0);
    // Запуск управления LCD дисплеем по I2C
    LCD_begin();
    // Вывод приветственного сообщения
    LCD_print("Sine Wave and");
    LCD_setCursor(3, 1);
    LCD_print("PWM Generator");
}

// Вывод страницы с меню
void printDisplayMenu() {
    uint8_t i;
    uint8_t rowNumber = 0;
    LCD_clear();
    // Выводим отображаемые строки в меню
    for (i = MENU_TOP; i <= MENU_BOTTOM; i++) {
        if (!(_displayMenuState & (1 << i))) {
            continue;
        }

        assert(rowNumber < 2);
        LCD_setCursor(0, rowNumber);
        switch (i) {
            case MODE_UPDATE_FREQUENCY: {
                LCD_print("Frequency");
                break;
            }
            case MODE_UPDATE_DUTY_CYCLE: {
                LCD_print("Duty Cycle");
                break;
            }
            case MODE_UPDATE_SHIFT: {
                LCD_print("Shift");
                break;
            }
            case MODE_INFO: {
                LCD_print("Information");
                break;
            }
            case MODE_BLUETOOTH_CONNECTION: {
                LCD_print("Bluetooth");
                break;
            }
            default:
                // UNREACHABLE
                assert(false);
        }

        // Выводим курсор-указатель в зависимости от текущей строки на дисплее
        if (i == _displayMenuPoint) {
            LCD_setCursor(LCD_CHARS_IN_LINE - 2, rowNumber);
            LCD_print("<-");
        }
        rowNumber += 1;
    }
}

// Вывод страницы с редактированием частоты сигналов
void printFrequencyPage(const bool isInit) {
    // Буффер для перевода числа в строку
    char buffer[4];

    if (isInit) {
        // Если страница только что открыта, то
        // выводим неизменяемые строки страницы
        LCD_clear();
        LCD_home();
        LCD_print("Frequency:");
        LCD_setCursor(LCD_CHARS_IN_LINE - 2, 0);
        LCD_print("Hz");
        LCD_setCursor(0, 1);
        LCD_print("[");
        LCD_setCursor(LCD_CHARS_IN_LINE - 1, 1);
        LCD_print("]");
    }

    // Вывод текущего значения частоты
    sprintf(buffer, "%d", _pwmParametrs.frequency);
    LCD_setCursor(11, 0);
    LCD_print(buffer);

    // Вывод меры "заполненности" шкалы частоты (формат "[#####____]")
    int8_t hashCount = ((LCD_CHARS_IN_LINE - 2) * (_pwmParametrs.frequency - MIN_FREQUENCY)) /
                       (MAX_FREQUENCY - MIN_FREQUENCY);
    uint8_t i;
    for (i = 0; i < LCD_CHARS_IN_LINE - 2; i++) {
        LCD_setCursor(i + 1, 1);
        LCD_print(i < hashCount ? "#" : " ");
    }
}

// Вывод страницы с редактированием коэффициента заполнения (скважности) сигнала
void printDutyCyclePage(const bool isInit) {
    // Буффер для перевода числа в строку
    char buffer[4];

    if (isInit) {
        // Если страница только что открыта, то
        // выводим неизменяемые строки страницы
        LCD_clear();
        LCD_home();
        LCD_print("Duty Cycle:");
        sprintf(buffer, "%d%c", DUTY_CYCLE_QUARTER, '%');
        LCD_setCursor(1, 1);
        LCD_print(buffer);
        sprintf(buffer, "%d%c", DUTY_CYCLE_HALF, '%');
        LCD_setCursor(12, 1);
        LCD_print(buffer);
        LCD_setCursor(15, 0);
        LCD_print("%");
    }

    // Вывод текущего значения коэффициента заполнения
    sprintf(buffer, "%d", _pwmParametrs.dutyCycle);
    LCD_setCursor(13, 0);
    LCD_print(buffer);

    // Вывод курсора-указателя на выбранное значение
    switch (_pwmParametrs.dutyCycle) {
        case DUTY_CYCLE_QUARTER: {
            LCD_setCursor(4, 1);
            LCD_print("<-");
            LCD_setCursor(10, 1);
            LCD_print("  ");
            break;
        }
        case DUTY_CYCLE_HALF: {
            LCD_setCursor(10, 1);
            LCD_print("->");
            LCD_setCursor(4, 1);
            LCD_print("  ");
            break;
        }
        default:
            // UNREACHABLE
            assert(false);
    }
}

// Вывод страницы с редактированием сдвига сигналов
void printShiftPage(const bool isInit) {
    // Буффер для перевода числа в строку
    char buffer[4];

    if (isInit) {
        // Если страница только что открыта, то
        // выводим неизменяемые строки страницы
        LCD_clear();
        LCD_home();
        LCD_print("Shift:");
        sprintf(buffer, "%.1fms", (float)(SHIFT_SMALL / 1000.0));
        LCD_setCursor(1, 1);
        LCD_print(buffer);
        sprintf(buffer, "%.1fms", (float)(SHIFT_BIG / 1000.0));
        LCD_setCursor(10, 1);
        LCD_print(buffer);
        LCD_setCursor(14, 0);
        LCD_print("ms");
    }

    // Вывод текущего значения сдвига
    sprintf(buffer, "%.1f", (float)(_pwmParametrs.shift / 1000.0));
    LCD_setCursor(11, 0);
    LCD_print(buffer);

    // Вывод курсора-указателя на выбранное значение
    switch (_pwmParametrs.shift) {
        case SHIFT_SMALL: {
            LCD_setCursor(6, 1);
            LCD_print("<-");
            LCD_setCursor(8, 1);
            LCD_print("  ");
            break;
        }
        case SHIFT_BIG: {
            LCD_setCursor(8, 1);
            LCD_print("->");
            LCD_setCursor(6, 1);
            LCD_print("  ");
            break;
        }
        default:
            // UNREACHABLE
            assert(false);
    }
}

// Вывод страницы со всеми параметрами сигналов
void printInfoPage(const bool isInit) {
    if (isInit) {
        // Если страница только что открыта, то
        // выводим неизменяемые строки страницы
        LCD_clear();
        LCD_home();
        LCD_setCursor(2, 0);
        LCD_print("F");
        LCD_setCursor(7, 0);
        LCD_print("D");
        LCD_setCursor(13, 0);
        LCD_print("S");
        LCD_setCursor(3, 1);
        LCD_print("Hz");
        LCD_setCursor(8, 1);
        LCD_print("%");
        LCD_setCursor(14, 1);
        LCD_print("ms");
    }

    // Буффер для перевода числа в строку
    char buffer[4];
    // Выводим текущие параметры сигналов, которые могут изменятся
    // при управлении по Bluetooth
    sprintf(buffer, "%d", _pwmParametrs.frequency);
    LCD_setCursor(0, 1);
    LCD_print(buffer);
    sprintf(buffer, "%d", _pwmParametrs.dutyCycle);
    LCD_setCursor(6, 1);
    LCD_print(buffer);
    sprintf(buffer, "%.1f", (float)(_pwmParametrs.shift / 1000.0));
    LCD_setCursor(11, 1);
    LCD_print(buffer);
}

// Вывод страницы с ожиданием Bluetooth подключения
void printBluetoothPage() {
    LCD_clear();
    LCD_setCursor(0, 0);
    LCD_print("Waiting for");
    LCD_setCursor(2, 1);
    LCD_print("BLE connection");
}
