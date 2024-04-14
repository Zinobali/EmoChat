#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // qRegisterMetaType<User>("User");
    // qRegisterMetaType<User&>("User&");
    // qRegisterMetaType<User*>("User*");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
