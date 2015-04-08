#include "mainwindow.h"
#include <qtsingleapplication/src/QtSingleApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QtSingleApplication a(argc, argv);
    if (a.isRunning())
    {
        a.sendMessage("weak_up");
        return EXIT_SUCCESS;
    }
    MainWindow w;
    w.show();

    QObject::connect(&a, SIGNAL(messageReceived(const QString&)),
             &w, SLOT(messageReceived(const QString&)));

    return a.exec();
}
