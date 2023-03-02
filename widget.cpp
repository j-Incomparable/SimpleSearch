#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //无边框、工具窗体窗口、始终保存在最底部 WindowStaysOnBottomHint  WindowStaysOnTopHint
    this->setWindowFlags(Qt::FramelessWindowHint  | Qt::Tool);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明

    //初始化
    this->iniEdgeCombo();//combobox控件
    this->iniCustomTxt(m_fastApp); //数据文件
    this->iniCustomTxt(m_fastLink);

    //确保只初始化一次
    QFile configFile(QString(QCoreApplication::applicationDirPath()+ "/" + "config.ini"));
    if(!configFile.exists()){
        QSettings settings("config.ini",QSettings::IniFormat);
        settings.setIniCodec("utf-8");// 解决乱码
        settings.beginGroup("Configuration");
        settings.setValue("UpdateFileShortKey",QString("config"));
        settings.setValue("ResetWin",QString("复位"));
        settings.endGroup();
        settings.beginGroup("Coordinate");
        settings.setValue("XPos",QString::number(675));
        settings.setValue("YPos",QString::number(965));
        settings.endGroup();
        settings.sync();//同步
    }
    connect(ui->inputLineEdit, SIGNAL(returnPressed()), this, SLOT(on_searchBtn_clicked()));
}

Widget::~Widget()
{
    delete ui;
}

// @brief: "初始化列表"按键
void Widget::iniEdgeCombo()
{
    //设置按钮提示信息
    ui->topBtn->setToolTip("置顶");
    ui->dragBtn->setToolTip("拖动");
    ui->edgeCombo->setToolTip("浏览器类型");
    ui->transCombo->setToolTip("翻译类型");

    //添加edgeCombo内容
    ui->edgeCombo->addItem(QIcon(":/images/icons/Edge.ico"),"Bing");
    ui->edgeCombo->addItem(QIcon(":/images/icons/du.ico"),"百度");
    ui->edgeCombo->addItem(QIcon(":/images/icons/360.ico"),"360");

    //添加transCombo内容
    QSize size(15,15);//缩小翻译图标，图标有点不合适...
    ui->transCombo->setIconSize(size);
    ui->transCombo->addItem(QIcon(":/images/icons/trans.ico"),0);
    ui->transCombo->addItem("中文");
    ui->transCombo->addItem("英文");
}

//搜索按钮槽函数
void Widget::on_searchBtn_clicked()
{
    QString getText;
    getText = ui->inputLineEdit->text();//获取输入文本
    if(getText.isEmpty())
        return;

    QSettings getSettings("config.ini",QSettings::IniFormat);
    getSettings.setIniCodec("utf-8");// 解决乱码
    getSettings.beginGroup("Configuration");
    QString isConfig = getSettings.value("UpdateFileShortKey").toString();
    QString isReset = getSettings.value("ResetWin").toString();
    getSettings.endGroup();

    //链接更改，默认update修改，reset重置窗口位置
    if(isConfig == getText || "update" == getText)
        changeApplicationOrLink();
    else if(isReset == getText || "reset" == getText){
        getSettings.beginGroup("Coordinate");
        int ax = getSettings.value("XPos").toInt();
        int ay = getSettings.value("YPos").toInt();
        getSettings.endGroup();
        ui->inputLineEdit->clear();
        this->move(ax,ay);
    }
    else if(openApplicationOrLink(getText,m_fastApp)) //检测是否为打开应用程序操作
        ui->inputLineEdit->clear();
    else if(openApplicationOrLink(getText,m_fastLink)) //检测是否为打开快速链接操作
        ui->inputLineEdit->clear();
    else
        browseSearch(getText,ui->edgeCombo->currentText());//正常搜索操作
}

//初始化数据文件
void Widget::iniCustomTxt(const QString &fileName)
{
    QDir totalPath(m_dataPath + fileName);
    if(totalPath.exists())//文件存在
        return ;

    //不存在data文件夹情况
    QDir dataPath(m_dataPath);
    if(!dataPath.exists()){
//        dataPath = QCoreApplication::applicationDirPath();//5.15.2已被废弃
        dataPath.setPath(QCoreApplication::applicationDirPath());
        dataPath.mkdir("data");//在当前目录下创建一个名称为data的文件
    }

    //不存在txt文件情况
    if(fileName == m_fastApp){
        QFile file(m_appPath);
        if(file.open(QIODevice::Text|QIODevice::ReadWrite)){
            QTextStream out(&file);
            out<<tr("快捷名称\t\t\t程序路径(路径符用[/])\n");
            file.close();//关闭文件
        }
    }else if(fileName == m_fastLink){
        QFile file(m_linkPath);
        if(file.open(QIODevice::Text|QIODevice::ReadWrite)){
            QTextStream out(&file);
            out<<tr("快捷名称\t\t\t链接地址\n");
            file.close();//关闭文件
        }
    }
    ui->inputLineEdit->clear();
}

//打开应用程序或者打开链接
bool Widget::openApplicationOrLink(const QString &getText,const QString &fileName)
{
    QString filePath;
    if(fileName == m_fastApp)
        filePath = m_appPath;
    else if(fileName == m_fastLink)
        filePath = m_linkPath;

    QFile input(filePath);
    if(!input.open(QIODevice::Text | QIODevice::ReadWrite)){//打开文件
        if(!input.open(QIODevice::Text | QIODevice::ReadWrite))//再次尝试打开
            return false;
    }
    QString content,filterTitle;
    QTextStream stream(&input);//文件流对象
    QStringList textList;
    filterTitle=stream.readLine();//把标题读走
    while(!stream.atEnd()){
        content = stream.readLine();//逐行扫描
        textList = content.split("\t\t\t");//分割字符
        if(textList[0] == getText && textList[1]!= NULL){
            if(QDesktopServices::openUrl(QUrl(textList[1]))){//打开成功情况下
                input.close();
                return true;
            }
        }
        textList.clear();//清除读取到的数据，继续读取数据
    }
    input.close();
    return false;
}

//修改信息
bool Widget::changeApplicationOrLink()
{
    //创建用于编辑文件的窗口
    FormTxt *txtWidget=new FormTxt(this);
    txtWidget->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除！！!
    txtWidget->setWindowTitle("修改文件数据");
    txtWidget->setWindowFlag(Qt::Window,true);//只有一个关闭按键

    //设置窗口位置
    QDesktopWidget *desktop = QApplication::desktop();
    txtWidget->move((desktop->width()-txtWidget->width())/2,(desktop->height()-txtWidget->height())/2);
    txtWidget->show();

    ui->inputLineEdit->clear();
    return true;
}

/***********************************************************************
 * @brief: 对输入框中的数据添加至浏览器搜索
 * @param:const QString &inputText,const QString &edgeType,查询数据和浏览器类型
 * @note: 优先跳转至翻译
 ***********************************************************************/
void Widget::browseSearch(const QString &inputText,const QString &edgeType)
{
    QString transUrl;
    transType(transUrl);//获取翻译类型
    if(!transUrl.isEmpty()){
        if(QDesktopServices::openUrl(QUrl(transUrl+inputText))){//跳转至翻译
            ui->inputLineEdit->clear();
            return;
        }
    }
    //搜索内容，转到翻译
    if(edgeType == "Bing"){
        //打开失败保留输入
        if(!QDesktopServices::openUrl(QUrl(m_bingUrl + inputText)))
            return;
    }else if(edgeType == "百度"){
        if(!QDesktopServices::openUrl(QUrl(m_baiDuUrl + inputText)))
            return;
    }else if(edgeType == "360"){
        if(!QDesktopServices::openUrl(QUrl(m_360Url + inputText)))
            return;
    }
    ui->inputLineEdit->clear();
}

/***********************************************************************
 * @brief: 获取翻译类型
 * @param: QString &type
 * @note: 仅支持 中翻英 英翻中
 ***********************************************************************/
void Widget::transType(QString &type)
{
    if(ui->transCombo->currentText() == "中文")
        type = "中文";
    else if(ui->transCombo->currentText() == "英文")
        type = "英文";
    else
        type = "";
}

/***********************************************************************
 * @brief: 窗口置顶槽函数
 * @param:
 * @note: m_isTop是否置顶标识。
 *      ①调整图片是因为图片选取不佳，导致显示效果有差异
 *      ②问题解决：QWidget: Incompatible window flags: the window can't be on top and on bottom at the same time
 *               //先取消原窗口置底，再设置窗口置顶   //先取消原窗口置顶，再设置窗口置底
 *      ③ this->showNormal();//showFullScreen()和showNormal()只对顶级窗口的起作用
 ***********************************************************************/
#include <QDebug>
void Widget::on_topBtn_clicked()
{
    //窗口未置顶情况
    if(!m_isTop){
        m_isTop=true;//窗口已置顶
        QIcon icon(":/images/icons/top.ico");
        QSize size(29,19);//置顶图片大小
        ui->topBtn->setIconSize(size);
        ui->topBtn->setIcon(icon);

        //先取消原窗口置底，再设置窗口置顶
//        this->setWindowFlags(this->windowFlags()&~Qt::WindowStaysOnBottomHint);
        this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnTopHint);
        this->show();//showFullScreen()和showNormal()只对顶级窗口的起作用
        qDebug()<<"=================";
    }else{
        m_isTop=false;//取消窗口置顶
        QIcon icon(":/images/icons/noTop.ico");
        QSize size(15,15);//默认图片大小
        ui->topBtn->setIconSize(size);
        ui->topBtn->setIcon(icon);

        //先取消原窗口置底，再设置窗口置顶
        this->setWindowFlags(this->windowFlags()&~Qt::WindowStaysOnTopHint);
//        this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnBottomHint);
        this->show();//showFullScreen()和showNormal()只对顶级窗口的起作用
        qDebug()<<"+++++++++++++++";
    }
}

//拖动按钮槽函数
void Widget::on_dragBtn_clicked()
{
    if(!m_isDrag){
        m_isDrag=true;//窗口可拖动
        QIcon icon(":/images/icons/drag.ico");
        ui->dragBtn->setIcon(icon);
        ui->inputLineEdit->setEnabled(false);
        ui->searchBtn->setEnabled(false);
        ui->inputLineEdit->setText("拖动完成先锁定！");
    }else{
        m_isDrag=false;//窗口不可拖动
        QIcon icon(":/images/icons/noDrag.ico");
        ui->dragBtn->setIcon(icon);
        ui->inputLineEdit->setEnabled(true);
        ui->searchBtn->setEnabled(true);
        ui->inputLineEdit->clear();
    }
}

//重写重绘函数，在paintEvent事件中绘制窗口边框。圆角实现
void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//抗锯齿
    painter.setBrush(QBrush(QColor(245,245,245)));
    painter.setPen(Qt::transparent);//透明画笔
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 15, 15);//rect:绘制区域  圆角弧度:15
    QWidget::paintEvent(event);
}

//窗口拖动：鼠标按压事件
void Widget::mousePressEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ClosedHandCursor);
    m_offset = event->globalPos() - this->pos(); // 这是个定值
//    QPoint offset = event->globalPos() - this->geometry().topLeft();
    QWidget::mousePressEvent(event);
}

//窗口拖动：鼠标释放事件
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
    this->m_offset = QPoint();
    QWidget::mouseReleaseEvent(event);
}

//窗口拖动：鼠标移动事件
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_isDrag)//不允许拖动
        return;

    //问题解决：防止按钮拖动导致的奇怪问题
    if(m_offset == QPoint())
        return;
//    QPoint offset = event->globalPos() - this->mZOffset;
    this->move(event->globalPos() - m_offset);
    QWidget::mouseMoveEvent(event);
}
