#include "SignalParameters.hpp"

#include <QMap>

// Установка параметров, полученных с устройства
void SignalParameters::initSignalParameters(QMap<QString, uint> data) noexcept
{
    QMap<QString, uint>::iterator dataIt;
    for (dataIt = data.begin(); dataIt != data.end(); dataIt++) {
        uint value = dataIt.value();
        if (dataIt.key() == "F") {
            if (minFrequency_ <= value && value <= maxFrequency_) {
                frequency_ = value;
                emit frequencyChanged();
            }
            else {
                assert(false);
            }
        }
        else if (dataIt.key() == "D") {
            if (value == smallDutyCycle_ || value == bigDutyCycle_) {
                dutyCycle_ = value;
                emit dutyCycleChanged();
            }
            else {
                assert(false);
            }
        }
        else if (dataIt.key() == "S") {
            if (value == smallShift_ || value == bigShift_) {
                shift_ = value;
                emit shiftChanged();
            }
            else {
                assert(false);
            }
        }
        else {
            assert(false);
        }
    }
}

// Формирование строки-параметров для отправки на устройство
QString SignalParameters::formatCurrentParameters() const noexcept
{
    return "F:" + QString::number(frequency_) + ";D:" + QString::number(dutyCycle_)
           + ";S:" + QString::number(shift_) + ";";
}
