#ifndef TSPINBOXDELEGATE_H
#define TSPINBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>

class TSpinBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TSpinBoxDelegate(QObject *parent = nullptr);//explicit关键字表示构造函数只能被直接显示调用，不能被隐式调用

    // QAbstractItemDelegate interface
public:
    //override 关键字表示重写基类的虚函数,基类是QAbstractItemDelegate的子类QStyledItemDelegate
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};



#endif // TSPINBOXDELEGATE_H
