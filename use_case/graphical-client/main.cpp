#include "mainwindow.h"

#include <QApplication>
#include <QIcon>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/logo_cacic.ico"));
    MainWindow w;
    w.show();
    return a.exec();
}
