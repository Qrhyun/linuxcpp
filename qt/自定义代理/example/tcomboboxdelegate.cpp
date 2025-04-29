#include "tcomboboxdelegate.h"
#include <QComboBox>
TComboBoxDelegate::TComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

void TComboBoxDelegate::setItems(QStringList items, bool editable)
{
    m_itemList=items;
    m_editable=editable;
}

//创建编辑器，createEditor():创建用于编辑模型数据的widget组件，如QSpinBox,QComboBox
QWidget *TComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox *editor=new QComboBox(parent);
    //setEditable()函数设置QComboBox是否可编辑
    editor->setEditable(m_editable);
    for(auto &item: m_itemList)
    {
        editor->addItem(item);//添加下拉列表项
    }

    return editor;
}

//设置编辑器的初始值，setEditorData():从数据模型获取数据，供widget组件进行编辑 。
void TComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox=dynamic_cast<QComboBox*>(editor);
    QString str=index.model()->data(index,Qt::EditRole).toString();
    comboBox->setCurrentText(str);
}

//设置模型数据，setModelData():将编辑器中的数据保存到模型中
void TComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox=dynamic_cast<QComboBox*>(editor);
    QString str=comboBox->currentText();
    model->setData(index,str,Qt::EditRole);
}

//更新编辑器的几何形状，updateEditorGeometry():设置编辑器的大小和位置
void TComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
