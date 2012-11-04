#include <QProcess>
#include "linuxSystem.h"

#include <QtDeclarative/qdeclarative.h>

LinuxSystem::LinuxSystem(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    //setFlag(ItemHasNoContents, false);
}

LinuxSystem::~LinuxSystem()
{
}

void LinuxSystem::shutdown(int afterSec)
{
    //QString cmd = QString("shutdown -h -t %1").arg(afterSec);
    QString cmd = QString("halt");
    //system(cmd.toStdString().c_str());
    QProcess::execute(cmd);
}
