#ifndef TDIALOGDATA_H
#define TDIALOGDATA_H

#include <QDialog>
#include <QSqlRecord>//在弹出窗口中仅仅记录修改项，与数据库的交互工作交给主窗口完成

namespace Ui {
class TDialogData;
}

class TDialogData : public QDialog
{
    Q_OBJECT
private:
    QSqlRecord m_record;


public:
    explicit TDialogData(QWidget *parent = nullptr);
    ~TDialogData();

    QSqlRecord record();//记录函数
    void setRecord(const QSqlRecord &newRecord);

private slots:
    void on_btnSetPhoto_clicked();

    void on_btnClearPhoto_clicked();

private:
    Ui::TDialogData *ui;
};

#endif // TDIALOGDATA_H
