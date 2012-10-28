#ifndef LINUXSOUND_PLUGIN_H
#define LINUXSOUND_PLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

class LinuxSoundPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
    
public:
    void registerTypes(const char *uri);
};

#endif // LINUXSOUND_PLUGIN_H

