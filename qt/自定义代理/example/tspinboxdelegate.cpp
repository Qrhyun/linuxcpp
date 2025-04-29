#include "tspinboxdelegate.h"
#include <QSpinBox>
TSpinBoxDelegate::TSpinBoxDelegate(QObject *parent):QStyledItemDelegate(parent)
{

}

//创建spinbox编辑器，createEditor():创建用于编辑模型数据的widget组件，如QSpinBox,QComboBox
QWidget *TSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QSpinBox *editor = new QSpinBox(parent);
    editor->setFrame(false);//去掉边框
    editor->setMinimum(0);//设置最小值
    editor->setMaximum(500000);//设置最大值
    return editor;
}

//设置编辑器的初始值，setEditorData():从数据模型获取数据，供widget组件进行编辑 。
void TSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QSpinBox *spinBox=dynamic_cast<QSpinBox*>(editor);
    //获取模型数据，Qt::EditRole表示编辑角色，之所以toInt()是因为模型数据是int类型
    int value=index.model()->data(index,Qt::EditRole).toInt();
    spinBox->setValue(value);
}

//设置模型数据，setModelData():将编辑器中的数据保存到模型中
void TSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *spinBox=dynamic_cast<QSpinBox*>(editor);
    int value=spinBox->value();
    model->setData(index,value,Qt::EditRole);
}

//更新编辑器的几何形状，updateEditorGeometry():设置编辑器的大小和位置
void TSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);//设置编辑器的大小和位置与单元格相同
}
