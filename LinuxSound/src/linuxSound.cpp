
#include <QtDeclarative/qdeclarative.h>
#include <QProcess>
#include "linuxSound.h"

LinuxSound::LinuxSound(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    
    // setFlag(ItemHasNoContents, false);

    program = QString("aplay");
}

LinuxSound::~LinuxSound()
{
}

void LinuxSound::setSource(QString source)
{
    mSource = source;
}

void LinuxSound::setRate(int rate)
{
    if (rate == 22050 || rate == 44100) {
        mRate = rate;
    }
}

void LinuxSound::play()
{
    QStringList arguments;
    arguments << QString("-r %1").arg(mRate) <<  mSource;

    mProcess = new QProcess;
    if (mProcess == NULL) {
        qDebug() << "Out-of-memory" << __FILE__ << __LINE__;
        return;
    }
    mProcess->start(program, arguments);
    emit playStarted();
    mProcess->waitForFinished();
    /* playing */
}
