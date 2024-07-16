#include "BluetoothHandler.hpp"

#include <QRegularExpression>

// Имя HC-05
static const QString HC_NAME("SignalsGenerator");
// Сообщение о разрешении управления
static const QString CONTROL_ENABLED_MSG("CONTROL ENABLED");
// Сообщение о закрытии соединения
static const QString DISCONNECT_MSG("DISCONNECT");
// Сообщение об успешной команде
static const QString OK_MSG("OK");
// Сообщение о невыполненной команде
static const QString ERROR_MSG("ERROR");

BluetoothHandler::BluetoothHandler(AppState &appState, SignalParameters &signalParameters,
                                   QObject *parent) noexcept
    : QObject(parent)
    , appState_(&appState)
    , signalParameters_(&signalParameters)
{
    discoveryAgent_ = new QBluetoothDeviceDiscoveryAgent(this);
    socket_ = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    // Настройка таймеров
    timer_.setInterval(std::chrono::milliseconds(1000));
    connect(&timer_, &QTimer::timeout, this, &BluetoothHandler::sendPasswordToControl);
    disconnectTimer_.setInterval(std::chrono::milliseconds(1000));
    disconnectTimer_.setSingleShot(true);
    connect(&disconnectTimer_, &QTimer::timeout, this, [this]() {
        if (socket_) {
            socket_->close();
        }
    });

    // Подключение обработчиков к сигналам
    connect(discoveryAgent_, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
            &BluetoothHandler::deviceDiscovered);
    connect(socket_, &QBluetoothSocket::connected, this, &BluetoothHandler::connected);
    connect(socket_, &QBluetoothSocket::disconnected, this,
            &BluetoothHandler::disconnected);
    connect(socket_,
            QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this,
            &BluetoothHandler::errorOccurred);
    connect(socket_, &QBluetoothSocket::readyRead, this, &BluetoothHandler::dataRecieved);
}

// Установка соединения с устройством
void BluetoothHandler::startBluetoothConnection() noexcept
{
    // Если еще не подключились к HC-05
    if (!connected_) {
        discoveryAgent_->start();
        loadingConnected_ = true;
        emit loadingConnectedChanged();
    }
    // Если устройство еще не разрешило управление
    else if (!controlEnabled_) {
        timer_.start();
        loadingControlEnabled_ = true;
        emit loadingControlEnabledChanged();
    }
}

// Обработчик нахождения Bluetooth устройств
void BluetoothHandler::deviceDiscovered(const QBluetoothDeviceInfo &device) noexcept
{
    // Если найденное устройство - HC-05
    if (device.name() == HC_NAME) {
        // Останавливаем поиск устройств
        discoveryAgent_->stop();

        QBluetoothAddress address = device.address();
        QBluetoothUuid serviceUuid(QBluetoothUuid::SerialPort);
        // Подключаемся к устройству
        socket_->connectToService(address, serviceUuid);
    }
}

// Обработчик установки соединения с HC-05
void BluetoothHandler::connected() noexcept
{
    connected_ = true;
    emit connectedChanged();
    loadingConnected_ = false;
    emit loadingConnectedChanged();
    timer_.start();
    loadingControlEnabled_ = true;
    emit loadingControlEnabledChanged();
}

// Обработчик закрытия соединения с HC-05
void BluetoothHandler::disconnected() noexcept
{
    connected_ = false;
    emit connectedChanged();
    controlEnabled_ = false;
    emit controlEnabledChanged();
    loadingConnected_ = false;
    emit loadingConnectedChanged();
    loadingControlEnabled_ = false;
    emit loadingControlEnabledChanged();
    valuesInitFromDevice_ = false;
    if (timer_.isActive()) {
        timer_.stop();
    }
}

// Обработчик ошибок при подключении к HC-05
void BluetoothHandler::errorOccurred(QBluetoothSocket::SocketError) noexcept
{
    // Сообщаем пользователю об ошибке
    emit showError(tr("Не удалось подключиться к модулю HC-05!"));
    // Закрываем соединение
    socket_->close();
}

// Обработчик получаемых сообщений
void BluetoothHandler::dataRecieved() noexcept
{
    if (socket_ && connected_) {
        // Полученное сообщение
        QByteArray receivedData = socket_->readAll();
        // Преобразовываем в удобный для нас формат - QString
        QString receivedDataString = QString::fromUtf8(receivedData);
        // Если устройство слишком быстро отправляет сообщения,
        // то при обработке классом QBluetoothSocket они могут
        // объединиться в одно сообщение
        QStringList lines = receivedDataString.split("\r\n");

        // Если сообщение пустое - ничего не делаем
        if (lines.isEmpty()) {
            return;
        }

        for (const auto &line : lines) {
            // Если сообщение пустое - ничего не делаем
            if (line.isEmpty()) {
                continue;
            }

            // Если устройство еще не разрешило управление собой
            if (!controlEnabled_) {
                // Если пароль подошел
                if (line == CONTROL_ENABLED_MSG) {
                    // Разрешаем управление
                    controlEnabled_ = true;
                    emit controlEnabledChanged();
                    loadingControlEnabled_ = false;
                    emit loadingControlEnabledChanged();
                    // Останавливаем периодическую отправку пароля на устройство
                    if (timer_.isActive()) {
                        timer_.stop();
                    }
                }
                // Если пароль не подошел
                else if (line == ERROR_MSG) {
                    // Останавливаем периодическую отправку пароля на устройство
                    if (timer_.isActive()) {
                        timer_.stop();
                    }
                    loadingControlEnabled_ = false;
                    emit loadingControlEnabledChanged();
                    // Сообщаем пользователю о неправильности пароля
                    emit showError(tr("Неверный пароль!"));
                }
                else {
                    // Если другое сообщение - ошибка
                    assert(false);
                }
            }
            // Если устройство закрывает соединение
            else if (line == DISCONNECT_MSG) {
                // Закрываем сокет
                socket_->close();
            }
            // Если еще не получили исходные параметры устройства
            else if (!valuesInitFromDevice_) {
                // Формат получаемого сообщения
                QRegularExpression regex("F:(\\d+);D:(\\d+);S:(\\d+);");
                QRegularExpressionMatch match = regex.match(line);

                // Если получили правильное сообщение
                if (match.hasMatch()) {
                    // Устанавливаем параметры
                    QMap<QString, uint> data;
                    data.insert("F", match.captured(1).toInt());
                    data.insert("D", match.captured(2).toInt());
                    data.insert("S", match.captured(3).toInt());
                    signalParameters_->initSignalParameters(data);
                    valuesInitFromDevice_ = true;
                }
                else {
                    // Сообщаем пользователю, что не получилось получить параметры
                    showError(tr("Ошибка при получении исходных параметров!"));
                }
            }
            // Подтверждающие сообщения от устройства
            else {
                if (line == OK_MSG) {
                    // Если получили это сообщение - устройство установило новые параметры
                    return;
                }
                else if (line == ERROR_MSG) {
                    // Если получили это сообщение - устройство не смогло установить
                    // параметры
                    emit showError(tr("Возникла ошибка при обработке данных!"));
                }
                else {
                    // Другого сообщения быть не может
                    assert(false);
                }
            }
        }
    }
}

// Отправляем пароль
void BluetoothHandler::sendPasswordToControl() noexcept
{
    if (socket_ && connected_ && !controlEnabled_) {
        sendData(appState_->getPassword());
    }
}

// Отправляем обновленные параметры
void BluetoothHandler::sendSignalParameters() noexcept
{
    if (socket_ && connected_ && controlEnabled_) {
        sendData(signalParameters_->formatCurrentParameters());
    }
}

// Отправляем сообщение на устройство
void BluetoothHandler::sendData(const QString &message) const noexcept
{
    if (socket_) {
        // Для того, чтобы HC-05 принял сообщение необходимо добавить в конец строки
        // "\r\n"
        QString hcMessage = message + "\r\n";
        socket_->write(hcMessage.toUtf8());
    }
}

// Отключаемся от устройства
void BluetoothHandler::disconnectDevice() noexcept
{
    if (connected_ && controlEnabled_) {
        sendData(DISCONNECT_MSG);
        // Чтобы успеть отправить сообщение, закрываем соединение через 1 секунду
        disconnectTimer_.start();
    }
}
