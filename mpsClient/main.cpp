#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QTranslator>

#include "Share/AppState.hpp"
#include "Share/BluetoothHandler.hpp"
#include "Share/CppObjects.hpp"
#include "Share/SignalParameters.hpp"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Использование улучшенного рендеринга компонентов интерфейса
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    // Информационные "пометки" приложения
    QCoreApplication::setApplicationName("SignalHandler");
    QCoreApplication::setApplicationVersion("v1.0.0");
    QCoreApplication::setOrganizationName("dg");
    QCoreApplication::setOrganizationDomain("ru");

    // Создание приложения
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Инициализация необходимых объектов
    AppState appState(app, engine);
    SignalParameters signalParameters;
    BluetoothHandler bluetoothHandler(appState, signalParameters);
    // Формирование объекта, доступного из QML
    CppObjects cppObjects(app, appState, bluetoothHandler, signalParameters);

    int appStart = EXIT_SUCCESS;

    static CppObjects *singletonCppObjects = &cppObjects;
    {
        // Передача коллекции классов в QML
        qmlRegisterSingletonType<CppObjects>(
            "CppObjects", 1, 0, "CppObjects",
            [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
                Q_UNUSED(engine)
                Q_UNUSED(scriptEngine)
                QQmlEngine::setObjectOwnership(singletonCppObjects,
                                               QQmlEngine::CppOwnership);
                return singletonCppObjects;
            });

        // Добавление к объекту приложения пути к элементам интерфейса
        engine.addImportPath(QStringLiteral("qrc:/"));
        const QUrl url(QStringLiteral("qrc:/MainWindow.qml"));

        // Обработка сигнала создания и подключения объекта интерфейса к объекту
        // приложения
        QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreated, &app,
            [url](QObject *obj, const QUrl &objUrl) {
                if (obj == nullptr && url == objUrl) {
                    qWarning("Some problem with qml");
                    QCoreApplication::exit(EXIT_FAILURE);
                }
            },
            Qt::QueuedConnection);

        engine.load(url);
        appStart = app.exec();
    }

    return appStart;
}
