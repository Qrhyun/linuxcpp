#include "widget.h"
#include "ui_widget.h"
#include "tperson.h"
#include <QMetaProperty>

//实现布局类widget的构造函数
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //实例化两个TPerson指针,用到对象树，不需要单独删除boy的内存了
    boy=new TPerson("小明",this);
    girl=new TPerson("小丽",this);
    girl->setAge(20);
    //设置运行时动态属性,区分按长大一岁是boy还是girl
    boy->setProperty("sex","boy");
    girl->setProperty("sex","girl");
    //设置属性值
    boy->setProperty("age",10);
    boy->setProperty("score",70);
    //为spinBox按钮对象设置动态属性，为了指定当按下spinBox是指向boy还是girl
    ui->spinBoy->setProperty("isBoy",true);
    ui->spinGirl->setProperty("isBoy",false);
    //connect是QObject类里面的，所以是一个基础的函数，写在Widget里面
    connect(boy,SIGNAL(ageChanged(int)),this,SLOT(do_ageChanged(int)));
    connect(girl,SIGNAL(ageChanged(int)),this,SLOT(do_ageChanged(int)));
    //valueChanged是spinBox自带的信号函数
    connect(ui->spinBoy,SIGNAL(valueChanged(int)),this,SLOT(do_spinChanged(int)));
    connect(ui->spinGirl,SIGNAL(valueChanged(int)),this,SLOT(do_spinChanged(int)));
    //上述两个功能同步
    connect(boy,SIGNAL(ageChanged(int)),ui->spinBoy,SLOT(setValue(int)));
    connect(girl,SIGNAL(ageChanged(int)),ui->spinGirl,SLOT(setValue(int)));
}


Widget::~Widget()
{
    delete ui;
}

//下面为槽函数
//满足按长大一岁的处理
void Widget::do_ageChanged(int value)
{
    //sender返回发送信号的发送方的指针，这里是一个TPerson类型，所以强转
    TPerson * person=qobject_cast<TPerson*>(sender());
    QString str=QString("%1,%2,年龄=%3")
                      .arg(person->property("name").toString())
                      .arg(person->property("sex").toString())
                      .arg(value);//arg()，它用于将变量值插入到字符串中
    ui->plainTextEdit->appendPlainText(str);
}

//满足按上下箭头的处理
void Widget::do_spinChanged(int arg1)
{
    QSpinBox * spinBox=qobject_cast<QSpinBox *>(sender());
    //property函数会返回属性的值。
    if(spinBox->property("isBoy").toBool())
        boy->setAge(arg1);
    else
        girl->setAge(arg1);
}

//
void Widget::on_btnBoyInc_clicked()
{
    boy->incAge();
}

void Widget::on_btnGirlInc_clicked()
{
    girl->incAge();
}

void Widget::on_btnClear_clicked()
{
    ui->plainTextEdit->clear();
}

void Widget::on_btnClassInfo_clicked()
{
    const QMetaObject * meta=boy->metaObject();
    ui->plainTextEdit->appendPlainText(QString("类名称；%1\n").arg(meta->className()));

    ui->plainTextEdit->appendPlainText("属性：");
    //属性的偏移量
    for(int i=meta->propertyOffset();i<meta->propertyCount();i++)
    {
        const char* propName=meta->property(i).name();
        QString propValue=boy->property(propName).toString();
        ui->plainTextEdit->appendPlainText(QString("属性名称=%1，属性值=%2").arg(propName).arg(propValue));
    }

    ui->plainTextEdit->appendPlainText("\n类信息（classInfo）：");
    //类信息的偏移量
    for(int i=meta->classInfoOffset();i<meta->classInfoCount();i++)
    {
        QMetaClassInfo classInfo=meta->classInfo(i);
        ui->plainTextEdit->appendPlainText(
            QString("Name=%1,Value=%2").arg(classInfo.name()).arg(classInfo.value()));
    }
}

