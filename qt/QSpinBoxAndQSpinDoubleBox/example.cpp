#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //设置小数位数
    ui->spinPrice->setDecimals(1);

    //这里用函数指针
    this->connect(ui->spinNum,&QSpinBox::valueChanged,
                  this,&Widget::on_btnCal_clicked);
    this->connect(ui->spinPrice,&QDoubleSpinBox::valueChanged,
                  this,&Widget::on_btnCal_clicked);

}

Widget::~Widget()
{
    delete ui;
}

//QSpinBox和QDoubleSpinBox的valueChanged()信号可以直接连接到槽函数
//QSpinBox和QDoubleSpinBox获取值的方法是value()，返回值类型是int和float，设置值的方法是setValue()
{
    int num=ui->spinNum->value();
    float price=ui->spinPrice->value();
    float total=price*num;
    ui->spinTotal->setValue(total);
}


void Widget::on_spinDec_valueChanged(int arg1)
{
    ui->spinBin->setValue(arg1);
    ui->spinHex->setValue(arg1);
}


void Widget::on_spinBin_valueChanged(int arg1)
{
    ui->spinDec->setValue(arg1);
    ui->spinHex->setValue(arg1);
}


void Widget::on_spinHex_valueChanged(int arg1)
{
    ui->spinDec->setValue(arg1);
    ui->spinBin->setValue(arg1);
}

