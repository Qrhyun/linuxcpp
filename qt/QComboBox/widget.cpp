#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_btnclear_clicked()
{
    ui->plainTextEdit->clear();
}

// 通过QComboBox的addItem()函数添加单个项
void Widget::on_btnInitItems_clicked()
{
    QIcon icon;
    icon.addFile(":/icons/icons/aim.ico");
    ui->combItems->clear();
    for(int i=0;i<20;++i)
        ui->combItems->addItem(icon,QString("Item%1").arg(i));
}

// 通过QComboBox的setEditable()函数设置可编辑
void Widget::on_chkEditable_clicked(bool checked)
{
    ui->combItems->setEditable(checked);
}

//通过QComboBox的clear函数设置清空
void Widget::on_btnClear_clicked()
{
    ui->combItems->clear();
}


void Widget::on_btnInitCities_clicked()
{
    ui->comboCities->clear();
    //通过QMap的insert函数添加项
    QMap<QString,int> City_Zone;
    City_Zone.insert("北京",10);
    City_Zone.insert("沙河",20);
    City_Zone.insert("上海",50);
    City_Zone.insert("南昌",90);
    //foreach遍历QMap,将QMap中的键值添加到QStringList中
    foreach(const QString &str,City_Zone.keys())//City_Zone.keys()返回一个包含所有键的QStringList
        ui->comboCities->addItem(str,City_Zone.value(str));//根据键获取对应的值，并添加到QComboBox中
}


void Widget::on_combItems_currentTextChanged(const QString &arg1)
{
    ui->plainTextEdit->appendPlainText(arg1);
}


void Widget::on_comboCities_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    QString str=ui->comboCities->currentText()+ui->comboCities->currentData().toString();
    ui->plainTextEdit->appendPlainText(str);
}

