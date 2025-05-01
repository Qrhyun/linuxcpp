#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
class QAbstractItemView;
class QGroupBox;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
private:
    // 定义一个指向QAbstractItemView的指针
    QAbstractItemView * m_itemView=nullptr;
    // 功能：刷新UI界面，参数为当前的QGroupBox控件，即显示当前控件的组框
    void refreshToUI(QGroupBox *curGroupBox);

    int getDropActionIndex(Qt::DropAction actionType);
    Qt::DropAction getDropActionType(int index);
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_radioListWidget_clicked();

    void on_radioListSource_clicked();

    void on_radioTreeWidget_clicked();

    void on_radioTableWidget_clicked();

    void on_chkBoxAcceptDrops_clicked(bool checked);

    void on_chkBoxDragEnabled_clicked(bool checked);

    void on_comboMode_currentIndexChanged(int index);

    void on_comboDefaultAction_currentIndexChanged(int index);

private:
    Ui::Widget *ui;

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};


#endif // WIDGET_H
