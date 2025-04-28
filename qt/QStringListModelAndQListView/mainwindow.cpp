#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_strList<<"北京"<<"上海"<<"南昌"<<"南京"<<"成都";//往数据列表中添加数据
    m_model=new QStringListModel(this);//实例化数据模型model
    m_model->setStringList(m_strList);//model和数据列表关联
    ui->listView->setModel(m_model);//设置listView的模型为m_model。即listView和数据模型关联
    //设置listView的编辑触发器为双击和选中点击
    ui->listView->setEditTriggers(QAbstractItemView::DoubleClicked|
                                  QAbstractItemView::SelectedClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnIniList_clicked()
{
    m_model->setStringList(m_strList);
}


void MainWindow::on_btnListClear_clicked()
{
    //清空数据模型中的数据，这里是model操作，不像QTreeWidget那样在view上操作，这里model操作会显示在view上
    m_model->removeRows(0,m_model->rowCount());//清空数据模型中的数据
}


void MainWindow::on_chkEditable_clicked(bool checked)
{
    //设置listView的编辑触发器为双击和选中点击
    if(checked)
        ui->listView->setEditTriggers(QAbstractItemView::DoubleClicked|
                                      QAbstractItemView::SelectedClicked);
    
    else
        ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void MainWindow::on_btnListAppend_clicked()
{
    //model的insertRow方法在数据模型的末尾插入一行数据，rowCount()方法返回当前数据模型的行数,在这里表示在最后一项rowCount()-1的后面（rowCount()）即插入一行数据
    m_model->insertRow(m_model->rowCount());
    //通过QModelIndex来操作QStringListModel中的数据，QModelIndex是一个模型索引类，表示模型中某个数据项的索引位置。通过m_model->index(m_model->rowCount()-1)来获取当前数据模型的最后一行的索引位置
    QModelIndex index=m_model->index(m_model->rowCount()-1);
    //model的setData方法设置数据模型中指定索引位置的数据，Qt::DisplayRole表示显示角色，即在视图中显示的数据
    m_model->setData(index,"new item",Qt::DisplayRole);
    //通过ui->listView->setCurrentIndex(index)来设置listView的当前索引为刚刚插入的数据项的索引位置
    ui->listView->setCurrentIndex(index);
}


void MainWindow::on_btnListInsert_clicked()
{
    QModelIndex index=ui->listView->currentIndex();
    m_model->insertRow(index.row());
    m_model->setData(index,"new item",Qt::DisplayRole);
    ui->listView->setCurrentIndex(index);
}

// 这里的moveRow方法是将当前选中的行移动到上面一行，参数分别表示：源模型索引，源行号，目标模型索引，目标行号。这里的目标行号是源行号-1，即上移一行
void MainWindow::on_btnListMoveUp_clicked()
{
    QModelIndex index;
    int curRow=ui->listView->currentIndex().row();
    m_model->moveRow(index,curRow,index,curRow-1);
}

void MainWindow::on_btnListMoveDown_clicked()
{
    QModelIndex index;
    int curRow=ui->listView->currentIndex().row();
    m_model->moveRow(index,curRow,index,curRow+2);
}



void MainWindow::on_btnListSort_clicked(bool checked)
{
    if(checked)
        m_model->sort(0,Qt::AscendingOrder);//升序排列
    else
        m_model->sort(0,Qt::DescendingOrder);//降序排列
}


void MainWindow::on_btnTextClear_clicked()
{
    ui->plainTextEdit->clear();
}


void MainWindow::on_btnTextImport_clicked()
{
    ui->plainTextEdit->clear();
    QStringList tmpList=m_model->stringList();//获取数据模型中的数据列表
    for(int i=0;i<tmpList.size();++i)
    {
        ui->plainTextEdit->appendPlainText(tmpList.at(i));//将数据模型中的数据列表添加到文本编辑器中
    }
}


void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString str=QString::asprintf("模型索引：row=%d，column=%d;",index.row(),index.column());
    str+=m_model->data(index,Qt::DisplayRole).toString();
    ui->statusbar->showMessage(str);
}

