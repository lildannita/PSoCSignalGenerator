#pragma once

#include <QGuiApplication>
#include <QObject>

#include "AppState.hpp"
#include "BluetoothHandler.hpp"
#include "SignalParameters.hpp"
#include "qqml.h"

// Класс - контейнер объектов, доступных для QML
class CppObjects final : public QObject {
    Q_OBJECT
    Q_PROPERTY(AppState *appState MEMBER appState_ CONSTANT FINAL)
    Q_PROPERTY(SignalParameters *signalParameters MEMBER signalParameters_ CONSTANT FINAL)
    Q_PROPERTY(BluetoothHandler *bluetoothHandler MEMBER bluetoothHandler_ CONSTANT FINAL)
    QML_ELEMENT
    QML_SINGLETON

public:
    CppObjects(QGuiApplication &app, AppState &appState,
               BluetoothHandler &bluetoothHandler_, SignalParameters &signalParameters,
               QObject *parent = nullptr)
        : QObject{ parent }
        , app_(&app)
        , appState_(&appState)
        , bluetoothHandler_(&bluetoothHandler_)
        , signalParameters_(&signalParameters){};

private:
    QPointer<QGuiApplication> app_;
    QPointer<AppState> appState_;
    QPointer<BluetoothHandler> bluetoothHandler_;
    QPointer<SignalParameters> signalParameters_;
};
