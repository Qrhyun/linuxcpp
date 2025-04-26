#include "tperson.h"

TPerson::TPerson(QString name,QObject *parent)
    : QObject{parent},m_name{name}
{}

//析构时借助程序结束运行，QObject被析构，随之被析构
TPerson::~TPerson()
{
    qDebug("TPerson的对象被删除！");
}

//三个方法函数和发送信号的函数配合
int TPerson::age()
{
    return m_age;
}

void TPerson::setAge(quint8 ageValue)
{
    if(m_age!=ageValue){
       m_age=ageValue;
       emit ageChanged(m_age);
    }
}

void TPerson::incAge()
{
    ++m_age;
    emit ageChanged(m_age);
}
