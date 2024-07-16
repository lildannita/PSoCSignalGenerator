#pragma once

#include "AppState.hpp"
#include "SignalParameters.hpp"

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QBluetoothServiceInfo>

// Класс - обработчик Bluetooth-соединения
class BluetoothHandler final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connected MEMBER connected_ NOTIFY connectedChanged FINAL)
    Q_PROPERTY(
        bool controlEnabled MEMBER controlEnabled_ NOTIFY controlEnabledChanged FINAL)
    Q_PROPERTY(bool loadingConnected MEMBER loadingConnected_ NOTIFY
                   loadingConnectedChanged FINAL)
    Q_PROPERTY(bool loadingControlEnabled MEMBER loadingControlEnabled_ NOTIFY
                   loadingControlEnabledChanged FINAL)

signals:
    void connectedChanged();
    void controlEnabledChanged();
    void loadingConnectedChanged();
    void loadingControlEnabledChanged();
    void showError(const QString &message);

public:
    BluetoothHandler(AppState &appState, SignalParameters &signalParameters,
                     QObject *parent = nullptr) noexcept;
    ~BluetoothHandler() noexcept { disconnectDevice(); }

    Q_INVOKABLE void startBluetoothConnection() noexcept;
    Q_INVOKABLE void sendSignalParameters() noexcept;
    Q_INVOKABLE void disconnectDevice() noexcept;

private slots:
    void connected() noexcept;
    void disconnected() noexcept;
    void deviceDiscovered(const QBluetoothDeviceInfo &device) noexcept;
    void errorOccurred(QBluetoothSocket::SocketError) noexcept;
    void dataRecieved() noexcept;
    void sendPasswordToControl() noexcept;

private:
    QPointer<AppState> appState_;
    QPointer<SignalParameters> signalParameters_;
    QPointer<QBluetoothDeviceDiscoveryAgent> discoveryAgent_;
    QPointer<QBluetoothSocket> socket_;
    QTimer timer_;
    QTimer disconnectTimer_;
    bool connected_ = false;
    bool controlEnabled_ = false;
    bool loadingConnected_ = false;
    bool loadingControlEnabled_ = false;
    bool valuesInitFromDevice_ = false;

    void sendData(const QString &message) const noexcept;
};
