#ifndef TDIALOGLOCATE_H
#define TDIALOGLOCATE_H

#include <QDialog>

namespace Ui {
class TDialogLocate;
}

class TDialogLocate : public QDialog
{
    Q_OBJECT

public:
    void setSpinRange(int rowCount,int colCount);
    
public slots:
//这里不只是初始化函数，而且要实现点击按钮，自定义对话框单元格的内容发生变化，所以这里也是一个槽函数
    void setSpinValue(int rowNo,int ColNo);

public:
    explicit TDialogLocate(QWidget *parent = nullptr);
    ~TDialogLocate();

private slots:
    void on_btnSetText_clicked();
signals:
    void changeCellText(int row,int column,QString &text);
    void changeActionEnable(bool en);

private:
    Ui::TDialogLocate *ui;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
};



#endif // TDIALOGLOCATE_H
