#include "linuxSystem_plugin.h"
#include "linuxSystem.h"

#include <QtDeclarative/qdeclarative.h>

void LinuxSystemPlugin::registerTypes(const char *uri)
{
    // @uri com.QTpoint.qmlcomponents
    qmlRegisterType<LinuxSystem>(uri, 1, 0, "LinuxSystem");
}

Q_EXPORT_PLUGIN2(LinuxSystem, LinuxSystemPlugin)

