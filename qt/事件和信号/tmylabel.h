#ifndef TMYLABEL_H
#define TMYLABEL_H

#include <QLabel>
#include <QObject>

class TMyLabel : public QLabel
{
    Q_OBJECT
public:
    TMyLabel(QWidget *parent=nullptr);//对象树
signals:
    //定义一个信号，信号的名称为doubleClicked，其封装的事件函数为QLabel类的mouseDoubleClickEvent函数
    void doubleClicked();

    // QWidget interface
protected:
    //要封装的是QLabel类的事件函数，重写了QLabel类的mouseDoubleClickEvent函数
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    // QObject interface
public:
    //重写了QObject类的event函数，封装了QLabel类的event函数
    virtual bool event(QEvent *event) override;
};





#endif // TMYLABEL_H
