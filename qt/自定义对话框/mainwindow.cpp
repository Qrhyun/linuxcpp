#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tdialogsize.h"
#include<QStandardItemModel>
#include<QItemSelectionModel>
#include "tdialogheaders.h"
#include <tdialoglocate.h>
#include <QLabel>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置model
    m_model=new QStandardItemModel(4,4,this);
    QStringList header;
    header<<"姓名"<<"性别"<<"学位"<<"部门";
    m_model->setHorizontalHeaderLabels(header);
    //设置selectionModel
    m_selection=new QItemSelectionModel(m_model);
    //绑定model，selectionModel 与视图
    ui->tableView->setModel(m_model);
    ui->tableView->setSelectionModel(m_selection);

    setCentralWidget(ui->tableView);

    labCellPos=new QLabel("当前单元格：",this);
    labCellPos->setMinimumWidth(200);

    labCellText=new QLabel("单元格内容：",this);
    labCellText->setMinimumWidth(200);

    ui->statusbar->addWidget(labCellPos);
    ui->statusbar->addWidget(labCellText);
    connect(m_selection,&QItemSelectionModel::currentChanged,
            this,&MainWindow::do_model_currentChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//当主界面的action被触发时，调用该函数，显示出自定义的对话框
void MainWindow::on_actTab_SetSize_triggered()
{
    TDialogSize *dlgTableSize=new TDialogSize();

    dlgTableSize->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);//设置对话框为固定大小
    dlgTableSize->setRowColumn(m_model->rowCount(),m_model->columnCount());//将当前行列数传入对话框，这里就将从model获取值和自定义对话框接收主界面的参数结合在一起

    int ret=dlgTableSize->exec();//显示对话框，阻塞主界面，等待用户输入
    if(ret==QDialog::Accepted)//根据返回值判断用户是否点击了确定按钮
    {
        //如果点击了确定按钮，则获取对话框中的行列数，设置到model中，这里就将利用model的setRowCount和setColumnCount函数来设置行列数和自定义对话框的setRowColumn函数的返回值结合在一起
        m_model->setRowCount(dlgTableSize->rowCount());
        m_model->setColumnCount(dlgTableSize->columnCount());
    }
    else if(ret==QDialog::Rejected)//如果点击了取消按钮，则不做任何操作
    {
        m_model->setColumnCount(dlgTableSize->columnCount());//这里就将model的setColumnCount函数和自定义对话框的roeCount函数的返回值结合在一起
        m_model->setRowCount(dlgTableSize->rowCount());
    }
    delete dlgTableSize;//这里一旦这个自定义退出了，就会被删除掉，如果没有这句，那么只有在整个程序退出时才会被删除，在其他过程中只是隐藏了
}

//当用户点击了设置表头的action时，弹出自定义的对话框，显示出当前表头的内容，并可以修改表头的内容
void MainWindow::on_actTab_SetHeader_triggered()
{
    if(dlgHeaders==nullptr)
        dlgHeaders=new TDialogHeaders(this);//这里利用对象树

    QStringList strList;
    for(int i=0;i<m_model->columnCount();++i)//获取当前model的列数，对应的表头数
        strList.append(m_model->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString());//获取当前model的表头数据，Qt::Horizontal表示横向的表头，Qt::DisplayRole表示显示的角色
    dlgHeaders->setHeaderList(strList);

    int ret=dlgHeaders->exec();
    if(ret==QDialog::Accepted)
    {
        m_model->setHorizontalHeaderLabels(dlgHeaders->headerList());
    }
}

//当用户点击了定位的action时，弹出自定义的对话框，显示出当前单元格的内容，并可以修改单元格的内容
void MainWindow::on_actTab_Locate_triggered()
{
    TDialogLocate *dlgLocate=new TDialogLocate(this);
    dlgLocate->setAttribute(Qt::WA_DeleteOnClose);//这里通过设置自定义对话框的属性，来实现当关闭对话框时，自动删除这个对话框的对象，这样就不需要手动删除了
    dlgLocate->setWindowFlag(Qt::WindowStaysOnTopHint);//设置对话框在最上面

    //初始化对话框
    dlgLocate->setSpinRange(m_model->rowCount(),m_model->columnCount());
    QModelIndex curIndex=m_selection->currentIndex();
    if(curIndex.isValid())
        dlgLocate->setSpinValue(curIndex.row(),curIndex.column());

    //设置对话框的行列数和表头数据，这里就将model的rowCount和columnCount函数和自定义对话框的setSpinRange函数结合在一起
    connect(dlgLocate,&TDialogLocate::changeCellText,this,&MainWindow::do_setCellText);
    //处理非模态窗口的关闭事件和显示事件，主要是实现非模态窗口的功能
    connect(dlgLocate,&TDialogLocate::changeActionEnable,ui->actTab_Locate,&QAction::setEnabled);
    //主页面的信号和自定义对话框的信号进行连接，这里就将主页面的cellIndexChanged信号和自定义对话框的setSpinValue函数结合在一起
    connect(this,&MainWindow::cellIndexChanged,dlgLocate,&TDialogLocate::setSpinValue);
    dlgLocate->show();
    dlgLocate->setModal(false);
}

//下面和上面不同的是，不仅仅是model的设置，还要写槽函数，来实现对单元格的内容进行修改，这里就将model的setData函数和自定义对话框的setSpinValue函数结合在一起
void MainWindow::do_setCellText(int row, int column, QString &text)
{
    QModelIndex index=m_model->index(row,column);
    m_selection->clearSelection();
    m_selection->setCurrentIndex(index,QItemSelectionModel::Select);
    m_model->setData(index,text,Qt::DisplayRole);
}

//对应上面的connect(this,&MainWindow::cellIndexChanged,dlgLocate,&TDialogLocate::setSpinValue);
void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    emit cellIndexChanged(index.row(),index.column());
}

void MainWindow::do_model_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    if(current.isValid())
    {
        labCellPos->setText(QString::asprintf("当前单元格：%d行，%d列",
                                              current.row(),current.column()));
        QStandardItem *aItem=m_model->itemFromIndex(current);
        labCellText->setText("单元格内容："+aItem->text());
    }
}

