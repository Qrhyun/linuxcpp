#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
void MainWindow::selectData()
{
    //1.打开数据库进行查询
    QString str="SELECT empNo,Name,Gender,Birthday,Province,Department,"
            " Salary FROM employee ORDER BY empNo";
    qryModel=new QSqlQueryModel(this);//创建查询模型
    qryModel->setQuery(str);//设置查询语句
    if(qryModel->lastError().isValid())
    {
        QMessageBox::critical(this,"错误","数据库查询错误\n"+
                              qryModel->lastError().text());
        return;
    }

    ui->statusBar->showMessage(QString("记录条数：%1").arg(qryModel->rowCount()));

    //2.视图和模型的连接
    ui->tableView->setModel(qryModel);
    selModel=new QItemSelectionModel(qryModel,this);
    ui->tableView->setSelectionModel(selModel);
    //3.处理表头
    QSqlRecord rec=qryModel->record();
    qryModel->setHeaderData(rec.indexOf("empNo"),Qt::Horizontal,"工号");
    qryModel->setHeaderData(rec.indexOf("Name"),   Qt::Horizontal, "姓名");
    qryModel->setHeaderData(rec.indexOf("Gender"), Qt::Horizontal, "性别");
    qryModel->setHeaderData(rec.indexOf("Birthday"),   Qt::Horizontal, "出生日期");
    qryModel->setHeaderData(rec.indexOf("Province"),   Qt::Horizontal, "省份");
    qryModel->setHeaderData(rec.indexOf("Department"), Qt::Horizontal, "部门");
    qryModel->setHeaderData(rec.indexOf("Salary"),     Qt::Horizontal, "工资");
    //4.映射，即数据库->model->widget的映射，在完成映射之后，还需要具体的动作槽函数来实现数据的提交和回滚，来实现具体的逻辑
    dataMapper=new QDataWidgetMapper(this);//创建数据映射器
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);//设置提交策略，即wight与model之间的关系也是双向的，然后设置为自动的
    dataMapper->setModel(qryModel);

    dataMapper->addMapping(ui->dbSpinEmpNo,rec.indexOf("empNo"));
    dataMapper->addMapping(ui->dbEditName,  rec.indexOf("Name"));
    dataMapper->addMapping(ui->dbComboSex,  rec.indexOf("Gender"));
    dataMapper->addMapping(ui->dbEditBirth, rec.indexOf("Birthday"));
    dataMapper->addMapping(ui->dbComboProvince, rec.indexOf("Province"));
    dataMapper->addMapping(ui->dbComboDep,      rec.indexOf("Department"));
    dataMapper->addMapping(ui->dbSpinSalary,    rec.indexOf("Salary"));

    dataMapper->toFirst();
    ui->actOpenDB->setEnabled(false);
    //5.设置当前行，实现第4步的映射
    connect(selModel,&QItemSelectionModel::currentRowChanged,
            this,&MainWindow::on_currentRowChanged);
}
//根据QDataWidgetMapper的当前行，获取当前QDataWidgetMapper的索引，根据前面获得的索引，设置QModel的当前索引，清空选择模型QSelectModel的选择，设置当前索引为QModel的当前索引
void MainWindow::refreshTableView()
{
    int index=dataMapper->currentIndex();
    QModelIndex curIndex=qryModel->index(index,1);
    selModel->clearSelection();
    selModel->setCurrentIndex(curIndex,QItemSelectionModel::Select);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//打开数据库，涉及数据库的内容显示在model中,以及model和QWidget的映射关系
void MainWindow::on_actOpenDB_triggered()
{
    QString aFile=QFileDialog::getOpenFileName(this,"选择文件","","SQLite数据库(*.db3)");
    if(aFile.isEmpty())
        return;

    DB=QSqlDatabase::addDatabase("QSQLITE");
    DB.setDatabaseName(aFile);
    if(DB.open())
        selectData();
    else
        QMessageBox::warning(this,"error","打开数据库失败");
}
///当选择行改变时，更新数据映射器的当前行
void MainWindow::on_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if(!current.isValid())
    {
        ui->dbLabPhoto->clear();
        ui->dbEditMemo->clear();
        return;
    }

    dataMapper->setCurrentModelIndex(current);
    bool first=(current.row()==0);
    bool last=(current.row()==qryModel->rowCount()-1);
    //1.根据当前行是否为第一行或最后一行，设置相应的动作可用性
    ui->actRecFirst->setEnabled(!first);
    ui->actRecPrevious->setEnabled(!first);
    ui->actRecNext->setEnabled(!last);
    ui->actRecLast->setEnabled(!last);

    //2.获取photo和memo（没有放入qryModel，因为上面并不是select *），处理照片
    /*
    QSelectModel获取当前行的索引
    QSqlRecord根据索引获取当前行的记录
    QSqlRecord的value方法获取当前行的某一列的值
    */
    int curRecNo=selModel->currentIndex().row();
    QSqlRecord curRec=qryModel->record(curRecNo);
    int empNo=curRec.value("EmpNo").toInt();//主键,向数据库中拿到数据

    QSqlQuery query;//创建查询对象
    query.prepare("select Memo,Photo from employee where EmpNo= :ID");
    query.bindValue(":ID",empNo);//绑定参数
    query.exec();//执行查询
    query.first();//获取第一条记录

    QVariant va=query.value("Photo");//获取照片，放在QVariant中
    if(!va.isValid())
        ui->dbLabPhoto->clear();
    else
    {
        QByteArray data=va.toByteArray();
        QPixmap pic;
        pic.loadFromData(data);
        ui->dbLabPhoto->setPixmap(pic.scaledToWidth(ui->dbLabPhoto->size().width()));
    }

    QVariant va2=query.value("Memo");//获取备注,放在QVariant中
    ui->dbEditMemo->setPlainText(va2.toString());
}

//首记录
void MainWindow::on_actRecFirst_triggered()
{
    dataMapper->toFirst();
    refreshTableView();
}


void MainWindow::on_actRecPrevious_triggered()
{
    dataMapper->toPrevious();
    refreshTableView();
}


void MainWindow::on_actRecNext_triggered()
{
    dataMapper->toNext();
    refreshTableView();
}


void MainWindow::on_actRecLast_triggered()
{
    dataMapper->toLast();
    refreshTableView();
}

