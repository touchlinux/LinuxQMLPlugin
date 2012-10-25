#ifndef LINUXINPUT_PLUGIN_H
#define LINUXINPUT_PLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

class LinuxInputPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
    
public:
    void registerTypes(const char *uri);
};

#endif // LINUXINPUT_PLUGIN_H

