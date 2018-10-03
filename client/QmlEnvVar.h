#ifndef QML_ENV_VAR_H
#define QML_ENV_VAR_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class QmlEnvVar : public QObject
{
    Q_OBJECT
public:    
    Q_INVOKABLE static QString value(const QString &name);
    Q_INVOKABLE static void setValue(const QString &name, const QString &value);
    Q_INVOKABLE static void unset(const QString &name);
};

// Define the singleton type provider function (callback).
QObject *qml_env_var_singletontype_provider(QQmlEngine *, QJSEngine *);

#endif // QML_ENV_VAR_H
