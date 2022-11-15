#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //编码
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

    MainWindow w;
    //设置窗口图标
    w.setWindowIcon(QIcon(":/Icon/res/Icon.ico"));

    w.show();

    return a.exec();
}
