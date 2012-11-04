#ifndef LINUXSYSTEM_H
#define LINUXSYSTEM_H

#include <QObject>
#include <QtDeclarative/QDeclarativeItem>

class LinuxSystem : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(LinuxSystem)
public:
    LinuxSystem(QDeclarativeItem *parent = 0);
    ~LinuxSystem();
#if QT_VERSION >= 0x040700
    static void registerQMLTypes()
    {
        qmlRegisterType<LinuxSystem>("LinuxSystem", 1, 0, "LinuxSystem");
    }
#endif
public slots:
    void shutdown(int);
};

QML_DECLARE_TYPE(LinuxSystem)

#endif // LINUXSYSTEM_H

