#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setStyle(QStyleFactory::create("Fusion"));
    a.setApplicationName("cartridges");
    a.setOrganizationName("AmGPGU");
    MainWindow w;
    w.setWindowTitle(QObject::tr("АмГПГУ - картриджи"));
    a.setWindowIcon(QIcon(":/null.png"));
    w.showMaximized();
    return a.exec();
}
