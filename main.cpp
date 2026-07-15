#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置应用程序样式
    a.setStyle("Fusion");
    // 设置应用程序图标
    a.setWindowIcon(QIcon(":/ico/gochess.ico"));
    MainWindow w;
    w.show();
    return a.exec();
}
