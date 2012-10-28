#include "linuxSound_plugin.h"
#include "linuxSound.h"

#include <QtDeclarative/qdeclarative.h>

void LinuxSoundPlugin::registerTypes(const char *uri)
{
    // @uri com.touchlinux.AudioEffect
    qmlRegisterType<LinuxSound>(uri, 1, 0, "LinuxSound");
}

Q_EXPORT_PLUGIN2(LinuxSound, LinuxSoundPlugin)

