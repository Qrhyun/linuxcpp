#include "dialog.h"
#include <QHBoxLayout>//水平布局管理器
#include <QVBoxLayout>//垂直布局管理器
#include <QCheckBox>//一个复选框控件
#include <QRadioButton>//单选按钮控件,其他选项会被自动取消选中
#include <QPlainTextEdit>//文本框
#include <QPushButton>

//下划线槽函数
void Dialog::do_chkBoxUnder(bool checked)
{
    QFont font=txtEdit->font();
    font.setUnderline(checked);
    txtEdit->setFont(font);
}

//斜体槽函数
void Dialog::do_chkBoxItalic(bool checked)
{
    QFont font=txtEdit->font();
    font.setItalic(checked);
    txtEdit->setFont(font);
}

//粗体槽函数
void Dialog::do_chkBoxBold(bool checked)
{
    QFont font=txtEdit->font();
    font.setBold(checked);
    txtEdit->setFont(font);
}

//颜色槽函数
void Dialog::do_setFontColor()
{
    QPalette plet=txtEdit->palette();
    if(radioBlack->isChecked())
        plet.setColor(QPalette::Text,Qt::black);
    if(radioRed->isChecked())
        plet.setColor(QPalette::Text,Qt::red);
    if(radioBlue->isChecked())
        plet.setColor(QPalette::Text,Qt::blue);
    txtEdit->setPalette(plet);
}

//构造函数
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    //初始化
    chkBoxUnder=new QCheckBox("下划线");
    chkBoxItalic=new QCheckBox("斜体");
    chkBoxBold=new QCheckBox("加粗");
    QHBoxLayout *HLay=new QHBoxLayout();
    HLay->addWidget(chkBoxUnder);
    HLay->addWidget(chkBoxItalic);
    HLay->addWidget(chkBoxBold);

    radioBlack=new QRadioButton("黑色");
    radioRed=new QRadioButton("红色");
    radioBlue=new QRadioButton("蓝色");
    QHBoxLayout *HLay2=new QHBoxLayout();
    HLay2->addWidget(radioBlack);
    HLay2->addWidget(radioRed);
    HLay2->addWidget(radioBlue);

    txtEdit=new QPlainTextEdit;
    txtEdit->setPlainText("Hello world\n 手工创建！");
    QFont font=txtEdit->font();
    font.setPointSize(20);
    txtEdit->setFont(font);

    btnOK=new QPushButton("确定");
    btnCancel=new QPushButton("取消");
    btnClose=new QPushButton("退出");
    QHBoxLayout *HLay3=new QHBoxLayout();
    HLay3->addStretch();
    HLay3->addWidget(btnOK);
    HLay3->addWidget(btnCancel);
    HLay3->addStretch();
    HLay3->addWidget(btnClose);

    QVBoxLayout *Vlay=new QVBoxLayout();
    Vlay->addLayout(HLay);
    Vlay->addLayout(HLay2);
    Vlay->addWidget(txtEdit);
    Vlay->addLayout(HLay3);

    setLayout(Vlay);//将这个垂直布局设置为窗口的主布局。都放在Dialog的时候，Diolog析构时候，随之析构

    //信号与槽函数的连接
    connect(chkBoxUnder,SIGNAL(clicked(bool)),this,SLOT(do_chkBoxUnder(bool)));
    connect(chkBoxItalic,SIGNAL(clicked(bool)),this,SLOT(do_chkBoxItalic(bool)));
    connect(chkBoxBold,SIGNAL(clicked(bool)),this,SLOT(do_chkBoxBold(bool)));

    //颜色信号与槽函数的连接
    connect(radioBlack,SIGNAL(clicked(bool)),this,SLOT(do_setFontColor()));
    connect(radioRed,SIGNAL(clicked(bool)),this,SLOT(do_setFontColor()));
    connect(radioBlue,SIGNAL(clicked(bool)),this,SLOT(do_setFontColor()));

    connect(btnOK,SIGNAL(clicked()),this,SLOT(accept()));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(btnClose,SIGNAL(clicked()),this,SLOT(close()));

    //改变窗口名称
    setWindowTitle("代码UI");
}

Dialog::~Dialog() {}
