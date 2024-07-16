#pragma once

#include <QGuiApplication>
#include <QObject>
#include <QPointer>
#include <QQmlApplicationEngine>
#include <QString>
#include <QTranslator>

// Класс - состояние приложения
class AppState final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isLinux READ isLinux CONSTANT FINAL)
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT FINAL)
    Q_PROPERTY(QString password MEMBER password_ NOTIFY passwordChanged FINAL)
    Q_PROPERTY(bool isRussianLanguage MEMBER isRussianLanguage_ NOTIFY
                   isRussianLanguageChanged FINAL)
    Q_PROPERTY(bool isConnected MEMBER isConnected_ NOTIFY isConnectedChanged FINAL)

signals:
    void passwordChanged();
    void isRussianLanguageChanged();
    void isConnectedChanged();

public:
    AppState(QGuiApplication &app, QQmlApplicationEngine &engine,
             QObject *parent = nullptr) noexcept;
    static bool isLinux() noexcept;
    static bool isAndroid() noexcept;

    Q_INVOKABLE void setPassword(const QString value) noexcept;
    Q_INVOKABLE void setIsRussianLanguage(const bool value) noexcept;
    QString getPassword() const noexcept { return password_; }

private:
    QString password_;
    bool isRussianLanguage_ = true;
    QString configName_{ "mpsClient.ini" };

    QPointer<QGuiApplication> app_;
    QPointer<QQmlApplicationEngine> engine_;

    QTranslator translator_;
    bool isTranslatorAvailible_ = false;
    bool isConnected_;

    void getLastSettingsFromConfig() noexcept;
    bool checkIfConfigInit() const noexcept;
    QMap<QString, QVariant> getConfigData() const noexcept;
    void updateConfigData(const QString key, const QVariant value) noexcept;
    void updateConfigData(QMap<QString, QVariant> values) noexcept;

    void translateApp();
};
