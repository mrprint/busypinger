#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include "QmlEnvVar.h"

int main(int argc, char *argv[])
{
    qmlRegisterSingletonType<QmlEnvVar>(
                "EnvVarModule",
                1,
                0,
                "EnvVar",
                qml_env_var_singletontype_provider
                );

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(QStringLiteral(":/utilities-system-monitor-4.ico")));

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
