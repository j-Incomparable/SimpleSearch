#include "widget.h"
#include <QApplication>
#include <QTextCodec>
#include <QSharedMemory>
#include <QSettings>
#include <qlabel.h>

int main(int argc, char *argv[])
{

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    QApplication a(argc, argv);

    static QSharedMemory *shareSimpleSearch = new QSharedMemory("SimpleSearchApp"); //创建【SingleApp】的共享内存块
    if (!shareSimpleSearch->create(1)){//创建大小1b的内存
        qApp->quit(); //创建失败，说明已经有一个程序运行，退出当前程序
        return -1;
    }

    Widget w;

    QSettings getSettings("config.ini",QSettings::IniFormat);
    getSettings.setIniCodec("utf-8");// 解决乱码
    getSettings.beginGroup("Coordinate");
    int ax = getSettings.value("XPos").toInt();
    int ay = getSettings.value("YPos").toInt();
    getSettings.endGroup();

    w.move(ax, ay);
    w.show();

    return a.exec();
}
