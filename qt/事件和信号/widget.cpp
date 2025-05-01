#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->label,&TMyLabel::doubleClicked,this,&Widget::do_doubleClick);
}

Widget::~Widget()
{
    delete ui;
}

//处理信号的槽函数
void Widget::do_doubleClick()
{
    ui->label->setText("标签被双击了！信号槽得到相应！");
    ui->label->adjustSize();
}

//封装事件的信号函数
void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    ui->label->setText("窗口被双击了！");
    ui->label->adjustSize();
}

