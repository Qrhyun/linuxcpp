#include "tdialogheaders.h"
#include "ui_tdialogheaders.h"
#include <QStringListModel>
#include <QMessageBox>
void TDialogHeaders::setHeaderList(QStringList &headers)
{
    // 接收主界面传入的QStringListModel里面存储的表头数据，然后自定义界面里面的QStringListModel进行存储，再显示到listView上
    m_model->setStringList(headers);
}

QStringList TDialogHeaders::headerList()
{
    return m_model->stringList();
}

TDialogHeaders::TDialogHeaders(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogHeaders)
{
    ui->setupUi(this);
    m_model=new QStringListModel(this);
    ui->listView->setModel(m_model);
}

TDialogHeaders::~TDialogHeaders()
{
    delete ui;
    QMessageBox::information(this,"提示","TDialogHeaders被删除！！！");
}
