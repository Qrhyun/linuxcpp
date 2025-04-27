#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->slider,&QSlider::valueChanged,this,&Widget::do_valueChanged);
    connect(ui->dial,&QSlider::valueChanged,this,&Widget::do_valueChanged);
    connect(ui->scrollBar,&QSlider::valueChanged,this,&Widget::do_valueChanged);
}

Widget::~Widget()
{
    delete ui;
}
//下面用到QPregressBar类的setValue()函数来设置进度条的值,setTextVisible()函数来设置是否显示文本, setInvertedAppearance()函数来设置是否反转显示进度条的外观, setFormat()函数来设置文本格式
void Widget::do_valueChanged(int value)
{
    ui->progressBar->setValue(value);
}


void Widget::on_chkBox_Visible_clicked(bool checked)
{
    ui->progressBar->setTextVisible(checked);
}


void Widget::on_chkBox_Inverted_clicked(bool checked)
{
    ui->progressBar->setInvertedAppearance(checked);
}


void Widget::on_radio_Percent_clicked()
{
    ui->progressBar->setFormat("%p%");
}


void Widget::on_radio_Value_clicked()
{
    ui->progressBar->setFormat("%v");
}

