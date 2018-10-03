#include "QmlEnvVar.h"
#include <stdlib.h>

QString QmlEnvVar::value(const QString& name)
{
    return qgetenv(qPrintable(name));
}

void QmlEnvVar::setValue(const QString& name, const QString &value)
{
    qputenv(qPrintable(name), value.toLocal8Bit());
}

void QmlEnvVar::unset(const QString& name)
{
    qunsetenv(qPrintable(name));
}

QObject *qml_env_var_singletontype_provider(QQmlEngine *, QJSEngine *)
{
    return new QmlEnvVar();
}
