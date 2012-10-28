#include <QtGui/QApplication>
#include <linuxSound.h>
#include "qmlapplicationviewer.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());

    LinuxSound::registerQMLTypes();

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer->setMainQmlFile(QLatin1String("qml/main.qml"));
    viewer->showExpanded();

    return app->exec();
}
