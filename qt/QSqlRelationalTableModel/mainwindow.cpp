#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
void MainWindow::openTable()
{
   //1.创建数据库连接，QSqlRelationalTableModel类
   tabModel=new QSqlRelationalTableModel(this,DB);
   //2.设置QSqlRelationalTableModel的各项属性
   tabModel->setTable("studInfo");
   tabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);//手动提交
   tabModel->setSort(tabModel->fieldIndex("studID"),Qt::AscendingOrder);
   //3.让QSqlRelationalTableModel和QTableView关联
   selModel=new QItemSelectionModel(tabModel,this);
   ui->tableView->setModel(tabModel);
   ui->tableView->setSelectionModel(selModel);
   //4.设置表头列的名称
   tabModel->setHeaderData(tabModel->fieldIndex("studID"),Qt::Horizontal,"学号");
   tabModel->setHeaderData(tabModel->fieldIndex("name"),    Qt::Horizontal, "姓名");
   tabModel->setHeaderData(tabModel->fieldIndex("gender"),  Qt::Horizontal, "性别");
   tabModel->setHeaderData(tabModel->fieldIndex("departID"),Qt::Horizontal, "学院");
   tabModel->setHeaderData(tabModel->fieldIndex("majorID"), Qt::Horizontal, "专业");

   //5.设置QSqlRelationalTableModel的外键关系,用QSqlRelation类来描述外键关系
   tabModel->setRelation(tabModel->fieldIndex("departID"),
                         QSqlRelation("departments","departID","department"));//关联到departments表中的department字段
   tabModel->setRelation(tabModel->fieldIndex("majorID"),
                         QSqlRelation("majors","majorID","major"));//关联到majors表中的major字段

   //6.用setItemDelegate()方法设置QSqlRelationalTableModel的代理类,并将其挂在到QTableView上                 
   ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
   //7.model->select()方法从数据库中读取数据
   tabModel->select();
   //8.设置QTableView的各项属性
   ui->actOpenDB->setEnabled(false);
   ui->actRecAppend->setEnabled(true);
   ui->actRecInsert->setEnabled(true);
   ui->actRecDelete->setEnabled(true);
   ui->actFields->setEnabled(true);
   //9.设置model和view的当前索引改变的信号槽函数
   connect(selModel,&QItemSelectionModel::currentChanged,this,
           &MainWindow::do_currentChanged);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->tableView);
    //设置QTableView的各项属性
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);//选择行为为选择单元格
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//选择模式为单选
    ui->tableView->setAlternatingRowColors(true);//交替行颜色
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 选择打开数据库
void MainWindow::on_actOpenDB_triggered()
{
    QString aFile=QFileDialog::getOpenFileName(this,"选择数据库","",
                                               "SQLite数据库(*.db3)");
    if(aFile.isEmpty())
        return;

    DB=QSqlDatabase::addDatabase("QSQLITE");
    DB.setDatabaseName(aFile);
    if(DB.open())
        openTable();
    else
        QMessageBox::warning(this,"error","打开数据库失败");
}

// 添加记录
void MainWindow::on_actRecAppend_triggered()
{
    tabModel->insertRow(tabModel->rowCount());
    selModel->clearSelection();
    QModelIndex curIndex=tabModel->index(tabModel->rowCount()-1,1);
    selModel->setCurrentIndex(curIndex,QItemSelectionModel::Select);//QItemSelectionModel::Select表示选中这个索引，这是枚举量
}

// 插入记录
void MainWindow::on_actRecInsert_triggered()
{
    QModelIndex curIndex=ui->tableView->currentIndex();
    tabModel->insertRow(curIndex.row());
    selModel->clearSelection();
    selModel->setCurrentIndex(curIndex,QItemSelectionModel::Select);//QItemSelectionModel::Select表示选中这个索引，这是枚举量
}

// 删除记录
void MainWindow::on_actRecDelete_triggered()
{
    tabModel->removeRow(selModel->currentIndex().row());
    tabModel->submitAll();//删除后提交数据
}

// 提交数据
void MainWindow::on_actSubmit_triggered()
{
    bool res=tabModel->submitAll();
    if(!res)
        QMessageBox::information(this,"info","数据提交失败！\n"
                                 +tabModel->lastError().text());
    else{
        //提交成功后，禁用提交和还原按钮
        ui->actSubmit->setEnabled(false);
        ui->actRevert->setEnabled(false);
    }
}
// 处理当前索引改变的信号槽函数
void MainWindow::do_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    ui->actSubmit->setEnabled(tabModel->isDirty());
    ui->actRevert->setEnabled(tabModel->isDirty());
}

// 还原数据
void MainWindow::on_actRevert_triggered()
{
    tabModel->revertAll();
    ui->actSubmit->setEnabled(false);
    ui->actRevert->setEnabled(false);
}

// 显示所有字段名称
void MainWindow::on_actFields_triggered()
{
    QSqlRecord emptyRec=tabModel->record();
    QString str;
    for(int i=0;i<emptyRec.count();++i)
    {
        str+=emptyRec.fieldName(i)+"\n";
    }
    QMessageBox::information(this,"所有字段名称",str);
}

