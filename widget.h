#ifndef WIDGET_H
#define WIDGET_H

#include "formtxt.h"

#include <QWidget>
#include <QCombobox>
#include <QPainter>
#include <QPaintEvent>
#include <QUrl>
#include <QDir>
#include <QTextStream>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QIcon>
#include <QSize>
#include <QStringList>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QSettings>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public:
    void iniEdgeCombo();//初始化combobox
    bool openApplicationOrLink(const QString &getText,const QString &fileName);//打开应用程序
    bool changeApplicationOrLink();//更改链接信息
    void browseSearch(const QString &inputText,const QString &edgeType);//浏览器搜索
    void iniCustomTxt(const QString &fileName);//初始化数据文件
    void transType(QString &type);//获取翻译类型

protected:
    virtual void paintEvent(QPaintEvent *event) override;//重绘函数
    virtual void mouseMoveEvent(QMouseEvent *event) override; //移动窗口
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_searchBtn_clicked();//获取lineText输入内容

    void on_topBtn_clicked();

    void on_dragBtn_clicked();

private:
    QPoint m_offset;//来记录鼠标相对于窗口的位置
    bool m_isTop=false;//是否置顶标识
    bool m_isDrag=false;//是否可拖曳标识

    //搜索链接
    QString m_bingUrl="https://cn.bing.com/search?ie=UTF-8&q=";
    QString m_baiDuUrl="http://www.baidu.com/s?ie=UTF-8&wd=";
    QString m_360Url="https://www.so.com/s?ie=utf8&q=";

    QString m_zhToen="https://fanyi.baidu.com/translate#zh/auto/";//中翻英
    QString m_enTozh="https://fanyi.baidu.com/translate#en/auto/";//英翻中

    //数据文件
    QString m_fastApp="应用快启.txt";
    QString m_fastLink="快速链接.txt";
    QString m_dataPath=QCoreApplication::applicationDirPath()+"/data";
    QString m_appPath=QCoreApplication::applicationDirPath()+"/data/" + m_fastApp;
    QString m_linkPath=QCoreApplication::applicationDirPath()+"/data/" + m_fastLink;

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
