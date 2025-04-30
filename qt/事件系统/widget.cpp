#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    grabKeyboard();//获取键盘输入焦点，允许该窗口接收键盘事件，捕获所有键盘输入
}

Widget::~Widget()
{
    delete ui;
}

//鼠标事件处理函数，QMouseEvent是Qt中鼠标事件的类（自生事件），函数的重载
void Widget::mousePressEvent(QMouseEvent *event)
{
    //这是派生类QWidget的mousePressEvent()函数，处理鼠标按下事件
    if(event->button()!=Qt::LeftButton)
        return;
    QPoint pt=event->pos();//相对widget的位置，返回鼠标相对于当前窗口部件（Widget）的坐标，类型为 QPoint
    QPointF relaPt=event->position();//返回鼠标相对于当前窗口部件的浮点坐标，类型为 QPointF
    QPointF winPt=event->scenePosition();// 返回鼠标相对于图形场景的坐标，类型为 QPointF。通常在使用图形视图框架（QGraphicsView）时才有意义
    QPointF globPt=event->globalPosition();//相对屏幕的绝对位置， 返回鼠标相对于整个屏幕的绝对坐标，类型为 QPointF
    QString str=QString::asprintf("pos()=(%d,%d)",pt.x(),pt.y());
    str+=QString::asprintf("\nposition()=(%.0f,%.0f)",relaPt.x(),relaPt.y());//%.0f,%.0f表示浮点数格式化输出，保留0位小数
    str+=QString::asprintf("\nscenePosition()=(%.0f,%.0f)",winPt.x(),winPt.y());
    str+=QString::asprintf("\nglobalPos()=(%.0f,%.0f)",globPt.x(),globPt.y());

    ui->labMove->setText(str);//ui->labMove 是一个标签控件（QLabel），用于显示鼠标位置信息。setText(str) 将标签的文本设置为之前构造的字符串
    ui->labMove->adjustSize();//adjustSize() 调整标签的大小，以适应其内容
    ui->labMove->move(event->pos());//move(event->pos()) 将标签移动到鼠标按下时的位置
    //这是基类QWidget的mousePressEvent()函数，调用基类的mousePressEvent()函数，用于处理鼠标按下事件。确保Qt的默认行为（如焦点转移等）能够正常执行。
    QWidget::mousePressEvent(event);
}

//键盘事件处理函数，QKeyEvent是Qt中键盘事件的类（自生事件）
void Widget::keyPressEvent(QKeyEvent *event)
{
    // ui->btnMove 是一个按钮控件（QPushButton），pos() 是 QWidget 类的成员函数，返回该控件相对于其父控件的坐标位置，类型为 QPoint
    //先记录当前按钮的位置
    QPoint pt=ui->btnMove->pos();
   
    if((event->key()==Qt::Key_A)||(event->key()==Qt::Key_Left))
    //move() 是 QWidget 类的成员函数，用于将控件移动到指定的位置。
        ui->btnMove->move(pt.x()-20,pt.y());

    if((event->key()==Qt::Key_D)||(event->key()==Qt::Key_Right))
        ui->btnMove->move(pt.x()+20,pt.y());

    if((event->key()==Qt::Key_W)||(event->key()==Qt::Key_Up))
        ui->btnMove->move(pt.x(),pt.y()-20);

    if((event->key()==Qt::Key_S)||(event->key()==Qt::Key_Down))
        ui->btnMove->move(pt.x(),pt.y()+20);

    QWidget::keyPressEvent(event);
}

//QWight的paintEvent处理事件，QPainter通常在QWidget的paintEvent()中使用
void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);//this指向当前的widget，但并不是对象树的用法
    painter.drawPixmap(0,0,width(),this->height(),
                       QPixmap(":/pics/images/background.jpg"));
}

//关闭事件处理函数，QCloseEvent是Qt中关闭事件的类（自生事件）
void Widget::closeEvent(QCloseEvent *event)
{
    //QMessageBox::question() 是一个静态函数，用于显示一个带有问题的消息框，返回用户的选择
    QMessageBox::StandardButton result=QMessageBox::question(
                this,"消息框","确定要退出吗？",
                QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

    if(result==QMessageBox::Yes)
        event->accept();
    else
        event->ignore();
}

