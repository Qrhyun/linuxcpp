#include "tdialoglocate.h"
#include "ui_tdialoglocate.h"
#include<QCloseEvent>
//自定义对话框的初始化函数，包括设置行列数和设置表头数据，这里也是接口函数，是根据主界面传入的参数进行设置的
void TDialogLocate::setSpinRange(int rowCount, int colCount)
{
    ui->spinBoxRow->setMaximum(rowCount-1);
    ui->spinBoxColumn->setMaximum(colCount-1);
}

void TDialogLocate::setSpinValue(int rowNo, int ColNo)
{
    ui->spinBoxRow->setValue(rowNo);
    ui->spinBoxColumn->setValue(ColNo);
}
//构造函数和析构函数
TDialogLocate::TDialogLocate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogLocate)
{
    ui->setupUi(this);
}

TDialogLocate::~TDialogLocate()
{
    delete ui;
}
//自定义对话框的接口函数，包括设置行列数和设置表头数据，用于与主页面进行交互，这里也是接口函数，是往主界面设置的
void TDialogLocate::on_btnSetText_clicked()
{
    QString text=ui->edtCaption->text();
    int row=ui->spinBoxRow->value();
    int column=ui->spinBoxColumn->value();

    if(ui->chkBoxRow->isChecked())
        ui->spinBoxRow->setValue(ui->spinBoxRow->value()+1);

    if(ui->chkBoxColumn->isChecked())
        ui->spinBoxColumn->setValue(ui->spinBoxColumn->value()+1);

    emit changeCellText(row,column,text);
}
//重写了关闭事件和显示事件，主要是实现非模态窗口的功能，这是前面的事件和信号的用法，自定义事件函数
void TDialogLocate::closeEvent(QCloseEvent *event)
{
    event->accept();
    //交给主窗口进行处理
    emit changeActionEnable(true);
}

void TDialogLocate::showEvent(QShowEvent *event)
{
    event->accept();
    //交给主窗口进行处理
    emit changeActionEnable(false);
}
