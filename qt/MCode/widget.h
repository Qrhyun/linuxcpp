#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
class TPerson;

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

private:
    //ui设计里面需要两个TPerson的对象指针
    TPerson * boy;
    TPerson * girl;
private slots:
    //获取信号函数后的槽函数，槽函数必须实现
    void do_ageChanged(int value);//年龄改变时候的槽函数
    void do_spinChanged(int arg1);//整体的界面功能的槽函数
    void on_btnBoyInc_clicked();

    void on_btnGirlInc_clicked();

    void on_btnClear_clicked();

    void on_btnClassInfo_clicked();

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
