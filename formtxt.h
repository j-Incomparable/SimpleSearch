#ifndef FORMTXT_H
#define FORMTXT_H

#include <QWidget>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#define FixedColumnCount 2  //文件固定2列

namespace Ui {
class FormTxt;
}

class FormTxt : public QWidget
{
    Q_OBJECT

public:
    explicit FormTxt(QWidget *parent = 0);
    ~FormTxt();

private slots:
    void on_actOpen_triggered();

    void on_actSave_triggered();

    void on_actInsert_triggered();

    void on_actAppend_triggered();

    void on_actDelete_triggered();

private:
    QString m_currFile;
    QString m_fastApp="应用快启.txt";
    QString m_fastLink="快速链接.txt";
    QString m_dataFolder="data";

    QStandardItemModel  *m_theModel;//数据模型
    QItemSelectionModel *m_theSelection;//Item选择模型

    void iniModelFromStringList(QStringList &fileContent);//从StringList初始化数据模型
    void iniNewItem(const QString &str);

private:
    Ui::FormTxt *ui;
};

#endif // FORMTXT_H
