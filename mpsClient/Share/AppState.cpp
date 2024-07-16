#include "AppState.hpp"

#include <QSettings>

AppState::AppState(QGuiApplication &app, QQmlApplicationEngine &engine,
                   QObject *parent) noexcept
    : QObject(parent)
    , app_(&app)
    , engine_(&engine)
{
    // Установка перевода
    isTranslatorAvailible_ = translator_.load(":/i18n/mpsClient_en_US");
    // Получение последних настроек из .ini файла
    getLastSettingsFromConfig();
}

// Если клиент - ПК
bool AppState::isLinux() noexcept
{
#if defined(__linux__) && !(defined(__ANDROID__) || defined(ANDROID))
    return true;
#else
    return false;
#endif
}

// Если клиент - Android
bool AppState::isAndroid() noexcept
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

// Установка исходных параметров
void AppState::getLastSettingsFromConfig() noexcept
{
    QMap<QString, QVariant> data;

    if (checkIfConfigInit()) {
        // Если конфиг - не пустой, то читаем и устанавливаем параметры
        data = getConfigData();
        isRussianLanguage_ = data.value("isRussianLanguage").toBool();
        password_ = data.value("password").toString();
    }
    else {
        // Если конфиг - пустой, то устанавливаем свои параметры и сохраняем в конфиг
        data.insert("isRussianLanguage", isRussianLanguage_);
        data.insert("password", password_);
        updateConfigData(data);
    }
}

// Проверка конфига на наличие данных
bool AppState::checkIfConfigInit() const noexcept
{
    QSettings config(configName_, QSettings::IniFormat);
    config.setIniCodec("UTF-8");
    return config.allKeys().size() > 0
           && config.childGroups().contains("GeneralSettings");
}

// Чтение данных из конфига
QMap<QString, QVariant> AppState::getConfigData() const noexcept
{
    QMap<QString, QVariant> data;
    QSettings config(configName_, QSettings::IniFormat);
    config.setIniCodec("UTF-8");

    config.beginGroup("GeneralSettings");
    data.insert("isRussianLanguage", config.value("isRussianLanguage").toBool());
    data.insert("password", config.value("password").toString());
    config.endGroup();

    return data;
}

// Сохранение данных в конфиг (для одного значения)
void AppState::updateConfigData(const QString key, const QVariant value) noexcept
{
    QSettings config(configName_, QSettings::IniFormat);
    config.setIniCodec("UTF-8");
    config.beginGroup("GeneralSettings");
    config.setValue(key, value);
    config.endGroup();
}

// Сохранение данных в конфиг (для нескольких значений)
void AppState::updateConfigData(QMap<QString, QVariant> values) noexcept
{
    QMap<QString, QVariant>::iterator valuesIt;
    QSettings config(configName_, QSettings::IniFormat);
    config.setIniCodec("UTF-8");

    config.beginGroup("GeneralSettings");
    for (valuesIt = values.begin(); valuesIt != values.end(); valuesIt++) {
        config.setValue(valuesIt.key(), valuesIt.value());
    }
    config.endGroup();
}

// Установка языка приложения
void AppState::setIsRussianLanguage(const bool value) noexcept
{
    // Если перевод не доступен, ничего не делаем
    if (!isTranslatorAvailible_) {
        isRussianLanguage_ = true;
        return;
    }

    // Меняем значение в конфиге
    isRussianLanguage_ = value;
    updateConfigData("isRussianLanguage", isRussianLanguage_);
    // Переводим приложения
    translateApp();
}

// Перевод приложения
void AppState::translateApp()
{
    if (isRussianLanguage_) {
        app_->removeTranslator(&translator_);
        engine_->retranslate();
    }
    else {
        app_->installTranslator(&translator_);
        engine_->retranslate();
    }
}

// Сохрание пароля в конфиг
void AppState::setPassword(const QString value) noexcept
{
    password_ = value;
    updateConfigData("password", password_);
}
