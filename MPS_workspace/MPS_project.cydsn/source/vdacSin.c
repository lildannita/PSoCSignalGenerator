#include "vdacSin.h"

// Размер пакета (1 байт)
#define BYTES_PER_BURST 1
// Пересылка пакета по запросу на входе drq
#define REQUEST_PER_BURST 1
// Адрес источника (память Flash)
#define SRC_BASE (CYDEV_FLASH_BASE)
// Адрес приемника (ЦАП)
#define DST_BASE (CYDEV_PERIPH_BASE)
// Размер массива точек синусоидального сигнала
#define VOLTAGE_ARRAY_LEN 128
// Частота Clock
#define DMA_CLOCK_FREQUENCY 3e6

// Канал DMA
uint8_t dmaChannel;
// Дескриптор транзакций
uint8_t dmaTd;

// Таблица значений синуса, посылаемых в ЦАП по каналу DMA
CYCODE const uint8 voltageWave[VOLTAGE_ARRAY_LEN] = {
    0x7F, 0x85, 0x8C, 0x92, 0x98, 0x9E, 0xA4, 0xAA, 0xB0, 0xB6, 0xBC, 0xC1, 0xC6, 0xCB, 0xD0, 0xD5, 0xDA, 0xDE, 0xE2, 0xE6,
    0xEA, 0xED, 0xF0, 0xF3, 0xF5, 0xF7, 0xF9, 0xFB, 0xFC, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFD, 0xFC, 0xFA, 0xF8, 0xF6,
    0xF4, 0xF1, 0xEE, 0xEB, 0xE8, 0xE4, 0xE0, 0xDC, 0xD7, 0xD3, 0xCE, 0xC9, 0xC4, 0xBE, 0xB9, 0xB3, 0xAD, 0xA7, 0xA1, 0x9B,
    0x95, 0x8F, 0x88, 0x82, 0x7C, 0x76, 0x6F, 0x69, 0x63, 0x5D, 0x57, 0x51, 0x4B, 0x45, 0x40, 0x3A, 0x35, 0x30, 0x2B, 0x27,
    0x22, 0x1E, 0x1A, 0x16, 0x13, 0x10, 0x0D, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
    0x03, 0x05, 0x07, 0x09, 0x0B, 0x0E, 0x11, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x29, 0x2E, 0x33, 0x38, 0x3D, 0x42, 0x48, 0x4E,
    0x54, 0x5A, 0x60, 0x66, 0x6C, 0x72, 0x79, 0x7F
};

// Инициализация компонентов для генерации синусоидального сигнала
void initVdacSin() {
    // Запуск ЦАП
	VDAC_Start();

    // Настройка канала
    dmaChannel = DMA_DmaInitialize(BYTES_PER_BURST, REQUEST_PER_BURST, HI16(SRC_BASE), HI16(DST_BASE));
    // Создание дескриптора транзакций
    dmaTd = CyDmaTdAllocate();
    // Настройка дескриптора транзакций
    CyDmaTdSetConfiguration(dmaTd, VOLTAGE_ARRAY_LEN, dmaTd, TD_INC_SRC_ADR);
    // Задание адресов источника и приемника
    CyDmaTdSetAddress(dmaTd, LO16((uint32)voltageWave), LO16((uint32)VDAC_Data_PTR));
    // Инициализация DMA
    CyDmaChSetInitialTd(dmaChannel, dmaTd);
    // Разрешение работы DMA
    CyDmaChEnable(dmaChannel, 1);
	
    // Установка исходной частоты сигнала
    handleSinSignalFrequencyChange();
}

// Обновление частоты синусоидального сигнала
void handleSinSignalFrequencyChange() {
    /*
    Для изменения частоты сигнала устнавливаем делитель на входной Clock:
        { ЧАСТОТА CLOCK } = ИСХОДНАЯ ЧАСТОТА CLOCK / ДЕЛИТЕЛЬ
        { ДЕЛИТЕЛЬ } = ИСХОДНАЯ ЧАСТОТА CLOCK / НЕОБХОДИМАЯ ЧАСТОТА СИНУСА
    При подстановке получаем:
        { ЧАСТОТА CLOCK } = ИСХОДНАЯ ЧАСТОТА CLOCK / (ИСХОДНАЯ ЧАСТОТА CLOCK / НЕОБХОДИМАЯ ЧАСТОТА СИНУСА)
    =>
        { ЧАСТОТА CLOCK } = НЕОБХОДИМАЯ ЧАСТОТА СИНУСА
    где
        { НЕОБХОДИМАЯ ЧАСТОТА СИНУСА } = КОЛ-ВО ТОЧЕК ОДНОГО ПЕРИОДА СИНУСА * УСТАНАВЛИВАЕМАЯ ЧАСТОТА
    */

	Sine_Clock_SetDivider(DMA_CLOCK_FREQUENCY / (VOLTAGE_ARRAY_LEN * _pwmParametrs.frequency));
}
