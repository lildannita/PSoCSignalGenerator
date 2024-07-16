#pragma once

#include <QObject>
#include <QString>

// Класс - обработчик изменения параметров сигналов
class SignalParameters final : public QObject {
    Q_OBJECT
    Q_PROPERTY(uint frequency READ getFrequencyValue WRITE setFrequencyValue NOTIFY
                   frequencyChanged FINAL)
    Q_PROPERTY(uint dutyCycle MEMBER dutyCycle_ NOTIFY dutyCycleChanged FINAL)
    Q_PROPERTY(uint shift MEMBER shift_ NOTIFY shiftChanged FINAL)

    Q_PROPERTY(uint minFrequency MEMBER minFrequency_ CONSTANT FINAL)
    Q_PROPERTY(uint maxFrequency MEMBER maxFrequency_ CONSTANT FINAL)
    Q_PROPERTY(uint smallDutyCycle MEMBER smallDutyCycle_ CONSTANT FINAL)
    Q_PROPERTY(uint bigDutyCycle MEMBER bigDutyCycle_ CONSTANT FINAL)
    Q_PROPERTY(uint smallShift MEMBER smallShift_ CONSTANT FINAL)
    Q_PROPERTY(uint bigShift MEMBER bigShift_ CONSTANT FINAL)

signals:
    void frequencyChanged();
    void dutyCycleChanged();
    void shiftChanged();

public:
    SignalParameters(QObject *parent = nullptr) noexcept
        : QObject(parent)
    {
    }

    uint getFrequencyValue() const noexcept { return frequency_; }
    void setFrequencyValue(uint value) noexcept
    {
        if (value < minFrequency_) {
            frequency_ = minFrequency_;
        }
        else if (value > maxFrequency_) {
            frequency_ = maxFrequency_;
        }
        else {
            frequency_ = value;
        }
        emit frequencyChanged();
    }
    void initSignalParameters(QMap<QString, uint> data) noexcept;
    QString formatCurrentParameters() const noexcept;

private:
    const uint minFrequency_ = 200;
    const uint maxFrequency_ = 450;
    const uint smallDutyCycle_ = 25;
    const uint bigDutyCycle_ = 50;
    const uint smallShift_ = 500;
    const uint bigShift_ = 2000;

    uint frequency_ = minFrequency_;
    uint dutyCycle_ = smallDutyCycle_;
    uint shift_ = smallShift_;
};
