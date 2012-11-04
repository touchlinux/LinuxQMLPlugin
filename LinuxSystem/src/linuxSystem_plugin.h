#ifndef LINUXSYSTEM_PLUGIN_H
#define LINUXSYSTEM_PLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

class LinuxSystemPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
    
public:
    void registerTypes(const char *uri);
};

#endif // LINUXSYSTEM_PLUGIN_H

