#include "widget.h"
#include "ui_widget.h"
#include <QMimeData>
#include<QFileInfo>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setAcceptDrops(true);//这行代码设置当前窗口（Widget）接受拖放操作
    ui->plainTextEdit->setAcceptDrops(false);//禁止 plainTextEdit 控件接受拖放操作。
    ui->label->setAcceptDrops(false);//禁止 label 控件接受拖放操作
    ui->label->setScaledContents(true);//设置 label 的内容（如图片）自动缩放以适应控件的大小。
}

Widget::~Widget()
{
    delete ui;
}

//自定义布局
void Widget::resizeEvent(QResizeEvent *event)
{
    QSize sz=ui->plainTextEdit->size();
    ui->plainTextEdit->move(5,5);
    ui->label->move(5,sz.height()+10);
    ui->plainTextEdit->resize(this->width()-10,sz.height());//this是当前窗口QWidget的指针
    ui->label->resize(this->width()-10,this->height()-sz.height()-20);
    event->accept();//调用accept()函数表示接受这个事件，告诉Qt框架这个事件已经被处理了。在Qt中，事件处理函数通常会调用accept()或ignore()来明确表示事件的处理状态。
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("dragEnterEvent事件，mimeData的formats:");
    for(auto &item:event->mimeData()->formats())
        ui->plainTextEdit->appendPlainText(item);
    ui->plainTextEdit->appendPlainText("dragEnterEvent事件，mimeData的urls:");
    for(auto &item:event->mimeData()->urls())
        ui->plainTextEdit->appendPlainText(item.path());

    if(event->mimeData()->hasUrls())
    {
        QString filename=event->mimeData()->urls().at(0).fileName();//获取文件名
        QFileInfo fileInfo(filename);//通过构造函数获取文件信息
        QString ex=fileInfo.suffix().toUpper();//获取文件后缀名并转换为大写，为下面的判断做准备
        if(ex=="JPG")
            event->acceptProposedAction();//接受这个东西进门
        else
            event->ignore();
    }else
        event->ignore();
}

void Widget::dropEvent(QDropEvent *event)
{
    QString fullpath=event->mimeData()->urls().at(0).path();
    fullpath=fullpath.right(fullpath.length()-1);
    QPixmap pixmap(fullpath);
    ui->label->setPixmap(pixmap);
    event->accept();
}
