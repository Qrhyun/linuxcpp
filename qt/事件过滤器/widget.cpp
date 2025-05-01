#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //乙方用installEventFilter()函数安装事件过滤器,this指向当前窗口QWidget对象
    ui->labSC->installEventFilter(this);
    ui->labDC->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

// 事件过滤器函数，watched是被过滤的对象，event是事件对象。有QEvent::Enter、QEvent::Leave、QEvent::MouseButtonPress、QEvent::MouseButtonRelease、QEvent::MouseButtonDblClick等事件类型
bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->labSC)
    {
        if(event->type()==QEvent::Enter)
            ui->labSC->setStyleSheet("background-color:rgb(170,255,255);");
        else if(event->type()==QEvent::Leave)
        {
            ui->labSC->setText("单击我");
            ui->labSC->setStyleSheet("");
        }else if(event->type()==QEvent::MouseButtonPress)
            ui->labSC->setText("button pressed");
        else if(event->type()==QEvent::MouseButtonRelease)
            ui->labSC->setText("button released");
    }

    if(watched==ui->labDC)
    {
        if(event->type()==QEvent::Enter)
            ui->labDC->setStyleSheet("background-color:rgb(85,255,127);");
        else if(event->type()==QEvent::Leave)
        {
            ui->labDC->setText("双击我");
            ui->labDC->setStyleSheet("");
        }else if(event->type()==QEvent::MouseButtonDblClick)
            ui->labDC->setText("button double clicked");
    }

    return QWidget::eventFilter(watched,event);
}
