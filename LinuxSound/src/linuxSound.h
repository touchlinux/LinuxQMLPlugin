#ifndef LINUXSOUND_H
#define LINUXSOUND_H

#include <QtDeclarative/QDeclarativeItem>
#include <QtMultimedia/QAudioOutput>
#include <QFile>
#include <QProcess>

class LinuxSound : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(LinuxSound)
    Q_PROPERTY(QString source READ source WRITE setSource)
    Q_PROPERTY(int rate READ rate WRITE setRate)
    
public:
    LinuxSound(QDeclarativeItem *parent = 0);
    ~LinuxSound();
#if QT_VERSION >= 0x040700
    static void registerQMLTypes()
    {
        qmlRegisterType<LinuxSound>("LinuxSound", 1, 0, "LinuxSound");
    }
#endif
    QString source() { return mSource; }
    void setSource(QString source);
    int rate() { return mRate; }
    void setRate(int rate);
    Q_INVOKABLE void play();
signals:
    void playStarted();
    void playFinished();
private:
    QString program;
    QString mSource;
    int mRate;
    QProcess *mProcess;
};

QML_DECLARE_TYPE(LinuxSound)

#endif // LINUXSOUND_H

