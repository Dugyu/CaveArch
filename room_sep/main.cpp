#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow m(0);                //只需要创建主界面即可
    m.show();
    return a.exec();
}
