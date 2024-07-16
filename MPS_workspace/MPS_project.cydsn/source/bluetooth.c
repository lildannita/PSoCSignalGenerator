#include "bluetooth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "lcd.h"
#include "signals.h"

// Размер принимаемого сообщения
#define BLE_INPUT_SIZE 50
// Пароль для разрешения управления устройством
#define PASSWORD "KDlp99nm!iu"
// Служебные сообщения
#define CONTROL_ENABLED "CONTROL ENABLED"
#define DISCONNECT "DISCONNECT"
#define OK "OK"
#define ERROR "ERROR"

// Ссылки на глобальные переменные
volatile uint8_t _state;
volatile uint8_t _changedParamsState;
struct PwmParametrs _pwmParametrs;

// Принимаемое сообщение
volatile char message[BLE_INPUT_SIZE + 1] = "\0";
volatile uint8_t message_index = 0;

// Объявление "приватных" функций
void handleBluetoothOkMessage();
void handleBluetoothErrorMessage();
void handleBluetoothMessage();
void handleComplexBluetoothMessage();
void sendMessage(const char* msg);
void sendInitParameters();

// Обработчик принимаемого байта данных
CY_ISR(BLE_Handler) {
    char c = BLE_UART_GetChar();

    // Последний символ, передаваемый с HC-05
    // -> обрабатываем сообщение
    if (c == '\n') {
        if (!(_state & (1 << BLUETOOTH_ENABLED))) {
            // Если управление по Bluetooth не разрешено
            if (strcmp(message, PASSWORD) == 0) {
                // Проверка, если не находимся на странице с ожиданием подключения
                if (!(_state & (1 << MODE_BLUETOOTH_CONNECTION))) {
                    // Такая ситуация невозможна - останавливаем работу программы
                    assert(false);
                }

                // Если сообщение = паролю
                // то разрешаем управление по Bluetooth
                _state |= 1 << BLUETOOTH_ENABLED;
                // и сообщаем клиенту, что пароль принят
                BLE_UART_PutString("CONTROL ENABLED\r\n");

				if (_state & (1 << MODE_BLUETOOTH_CONNECTION)) {
					printInfoPage(true);
				}

                // Очищаем сообщение
                message_index = 0;
                message[message_index] = '\0';
                sendInitParameters();
            } else {
                // Если пароль неверный - сообщаем пользователю
                handleBluetoothErrorMessage();
            }
        } else if (strcmp(message, DISCONNECT) == 0) {
            // Если клиент закрывает соединение
            _state &= ~(1 << BLUETOOTH_ENABLED);
            // Отображаем страницу с ожиданием подключения
            printBluetoothPage();
            // Очищаем сообщение
            message_index = 0;
            message[message_index] = '\0';
        } else {
            // Если управление разрешено, разбираем команду
            handleBluetoothMessage();
        }
        return;
    }
    // Предпоследний символ, передаваемый с HC-05
    //-> заканчиваем сохранение сообщения
    if (c == '\r') {
        message[message_index++] = '\0';
        return;
    }
    // Сохраняем переданный символ
    message[message_index++] = c;
}

// Обработчик окончания передачи сообщения клиенту
// (нужен только для закрытия соединения со стороны устройства)
CY_ISR(BLE_Closing_Handler) {
    // Запрещаем подключение по Bluetooth
    _state &= ~(1 << BLUETOOTH_ENABLED);
    // Выключаем UART модуль
    BLE_UART_Stop();
    // Выключаем прерывание окончания отправки сообщения
    BLE_ISR_CLOSING_Stop();
}

// Инициализация Bluetooth соединения
void initBluetoothConnection() {
    // Указываем обработчик прерывания
    BLE_ISR_StartEx(BLE_Handler);
    // Устанавливаем высший приоритет
    BLE_ISR_SetPriority(0);
    // Выключаем Bluetooth
    stopBluetoothConnection();
}

// Включение Bluetooth
void startBluetoothConnection() {
    // Включение UART модуля
    BLE_UART_Start();
    // Отображаем страницу с ожиданием подключения
    printBluetoothPage();
}

// Выключение Bluetooth
void stopBluetoothConnection() {
    if (_state & (1 << BLUETOOTH_ENABLED)) {
        // Если управление было передано клиенту, то
        // отправляем ему сообщение о закрытии соединения
        BLE_ISR_CLOSING_StartEx(BLE_Closing_Handler);
        sendMessage(DISCONNECT);
    } else {
        // Если управление не было передано клиенту, то сразу выключаем UART модуль
        BLE_UART_Stop();
    }
}

// Отправка клиенту текущих параметров устройства
void sendInitParameters() {
    // Формирование строки с параметрами
    sprintf(message, "F:%d;D:%d;S:%d;", _pwmParametrs.frequency, _pwmParametrs.dutyCycle,
            _pwmParametrs.shift);
    // Отправка сообщения
    sendMessage(message);
    // Очистка сообщения-буфера
    message[message_index] = '\0';
}

// Обработка неправильной команды от HC-05
void handleBluetoothErrorMessage() {
    // Очищаем сообщение
    message_index = 0;
    message[message_index] = '\0';
    // Сообщаем о неправильности команды
    sendMessage(ERROR);
}

// Сообщаем, что команда успешно обработана
void handleBluetoothOkMessage() {
    // Очищаем сообщение
    message_index = 0;
    message[message_index] = '\0';
    // Сообщаем об обработке команды
    sendMessage(OK);
}

// Обработка сообщения от HC-05
void handleBluetoothMessage() {
    // Длина сообщения
    uint8_t messageLength = strlen(message);
    // Изменяемый параметр сигналов
    char parameter = message[0];

    // Длина сообщения - минимум 3 значения, первое значение - символ,
    // второе значение - разделитель, далее - значение параметра
    if (messageLength < 3 || !isalpha(parameter) || message[1] != ':') {
        handleBluetoothErrorMessage();
        return;
    }

    // Значение после двоеточия - целое число
    for (message_index = 2; message_index < messageLength; message_index++) {
        if (!isdigit(message[message_index])) {
            // Если нет, то пробуем разобрать "большую строку", которую получаем при управлении от клиента
            handleComplexBluetoothMessage();
            return;
        }
    }

    // Новое значение для изменяемого параметра
    uint16_t value = atoi(&message[2]);
    switch (parameter) {
        // Изменение частоты сигналов
        case 'F': {
            if (MIN_FREQUENCY <= value && value <= MAX_FREQUENCY) {
                _pwmParametrs.frequency = value;
                _changedParamsState |= 1 << MODE_UPDATE_FREQUENCY;
            } else {
                handleBluetoothErrorMessage();
                return;
            }
            break;
        }
        // Изменение скважности сигналов
        case 'D': {
            if (value == DUTY_CYCLE_QUARTER || value == DUTY_CYCLE_HALF) {
                _pwmParametrs.dutyCycle = value;
                _changedParamsState |= 1 << MODE_UPDATE_DUTY_CYCLE;
            } else {
                handleBluetoothErrorMessage();
                return;
            }
            break;
        }
        // Изменение сдвига сигналов
        case 'S': {
            if (value == SHIFT_SMALL || value == SHIFT_BIG) {
                _pwmParametrs.shift = value;
                _changedParamsState |= 1 << MODE_UPDATE_SHIFT;
            } else {
                handleBluetoothErrorMessage();
                return;
            }
        }
        // Несуществующий параметр
        default: {
            handleBluetoothErrorMessage();
            return;
        }
    }

    // Сообщаем об обработке команды
    handleBluetoothOkMessage();
    // Изменяем текущие параметры сигналов
    handleSignalParamsChange();
    // Отображаем измененные значения
    printInfoPage(false);
}

// Обработка комплексного сообщения от клиента
void handleComplexBluetoothMessage() {
    // Временные переменные для получения значений параметров
    int tmp_F = 0;
    int tmp_D = 0;
    int tmp_S = 0;

    if (sscanf(message, "F:%d;D:%d;S:%d;", &tmp_F, &tmp_D, &tmp_S) != 3) {
        // Если сообщение не соответствует шаблону - сообщаем об ошибке
        handleBluetoothErrorMessage();
        return;
    }

    if (!(MIN_FREQUENCY <= tmp_F && tmp_F <= MAX_FREQUENCY) ||
        !(tmp_D == DUTY_CYCLE_QUARTER || tmp_D == DUTY_CYCLE_HALF) ||
        !(tmp_S == SHIFT_SMALL || tmp_S == SHIFT_BIG)) {
        // Если полученные значения не соответствуют условиям - сообщаем об ошибке
        handleBluetoothErrorMessage();
        return;
    }

    // Сохраняем полученные значения
    _pwmParametrs.frequency = tmp_F;
    _changedParamsState |= 1 << MODE_UPDATE_FREQUENCY;
    _pwmParametrs.dutyCycle = tmp_D;
    _changedParamsState |= 1 << MODE_UPDATE_DUTY_CYCLE;
    _pwmParametrs.shift = tmp_S;
    _changedParamsState |= 1 << MODE_UPDATE_SHIFT;

    // Сообщаем об обработке команды
    handleBluetoothOkMessage();
    // Изменяем текущие параметры сигналов
    handleSignalParamsChange();
    // Обновляем отображаемую на дисплее информацию
    printInfoPage(false);
}

// Отправка сообщений клиенту
void sendMessage(const char msg[]) {
    // Создаем новую строку - с указанием символов конца строки для HC-05
    int len = strlen(msg) + 3;
    char* correctMsg = malloc(len * sizeof(char));

    if (correctMsg == NULL) {
        // Ошибка при отладке, если строку не получилось создать
        assert(false);
    }

    // Формируем необходимое сообщение
    strcpy(correctMsg, (msg));
    strcat(correctMsg, "\r\n");

    // Отправляем сообщение клиенту
    BLE_UART_PutString(correctMsg);

    // Освобождаем выделенную память
    free(correctMsg);
}
