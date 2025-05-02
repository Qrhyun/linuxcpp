#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "tdialogdata.h"
void MainWindow::selectData()
{
    qryModel=new QSqlQueryModel(this);
    selModel=new QItemSelectionModel(qryModel,this);
    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(selModel);

    //QModel设置sql语句
    qryModel->setQuery("SELECT empNo,Name,Gender,Birthday,Province,Department,"
                       "Salary FROM employee order by empNo");

    if(qryModel->lastError().isValid())
    {
        QMessageBox::information(this,"error","数据库读取表格失败！\n"
                                 +qryModel->lastError().text());
        return;
    }
  
    QSqlRecord rec=qryModel->record();
    qryModel->setHeaderData(rec.indexOf("empNo"),Qt::Horizontal,"工号");
    qryModel->setHeaderData(rec.indexOf("Name"),Qt::Horizontal, "姓名");
    qryModel->setHeaderData(rec.indexOf("Gender"),Qt::Horizontal, "性别");
    qryModel->setHeaderData(rec.indexOf("Birthday"),Qt::Horizontal, "出生日期");
    qryModel->setHeaderData(rec.indexOf("Province"),Qt::Horizontal, "省份");
    qryModel->setHeaderData(rec.indexOf("Department"),Qt::Horizontal, "部门");
    qryModel->setHeaderData(rec.indexOf("Salary"),Qt::Horizontal, "工资");

    ui->actOpenDB->setEnabled(false);
    ui->actRecInsert->setEnabled(true);
    ui->actRecDelete->setEnabled(true);
    ui->actRecEdit->setEnabled(true);
    ui->actScan->setEnabled(true);
}

void MainWindow::updateRecord(int recNo)
{
    //1.记录当前选中记录的内容
    QSqlRecord curRec=qryModel->record(recNo);
    int empNo=curRec.value("EmpNo").toInt();
    //2.通过QSqlQuery对象的prepare函数，准备sql语句，准备往数据库中更新数据
    QSqlQuery query;
    query.prepare("select * from employee where EmpNo=:ID");
    query.bindValue(":ID",empNo);
    query.exec();
    query.first();
    if(!query.isValid())
        return;

    curRec=query.record();
    //3.打开子页面，传入一条记录
    TDialogData *dataDialog=new TDialogData(this);
    dataDialog->setRecord(curRec);//通过将curRec传入setRecord函数，设置数据
    if(dataDialog->exec()==QDialog::Accepted)
    {
        //4.在子页面中设置数据，返回到主页面
        QSqlRecord recData=dataDialog->record();
        query.prepare("update employee set Name=:Name, Gender=:Gender,"
                      " Birthday=:Birthday,  Province=:Province,"
                      " Department=:Department, Salary=:Salary,"
                      " Memo=:Memo, Photo=:Photo "
                      " where EmpNo = :EmpNo");
        query.bindValue(":EmpNo",recData.value("EmpNo"));
        query.bindValue(":Name",recData.value("Name"));
        query.bindValue(":Gender",recData.value("Gender"));
        query.bindValue(":Birthday",recData.value("Birthday"));
        query.bindValue(":Province",recData.value("Province"));
        query.bindValue(":Department",recData.value("Department"));
        query.bindValue(":Salary",recData.value("Salary"));
        query.bindValue(":Memo",recData.value("Memo"));
        query.bindValue(":Photo",recData.value("Photo"));

        if(!query.exec())
            QMessageBox::critical(this,"error","更新数据错误\n"+query.lastError().text());
        else
        {
             //qryModel->setQuery(qryModel->query().executedQuery());
            qryModel->query().exec();
        }
    }
    delete dataDialog;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->tableView);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//选择整行
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//单行选择
    ui->tableView->setAlternatingRowColors(true);//交替行颜色
}

MainWindow::~MainWindow()
{ 
    delete ui;
}

//打开数据库
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

//插入记录
void MainWindow::on_actRecInsert_triggered()
{
    QSqlQuery query;
    query.exec("select * from employee where EmpNo=-1");

    QSqlRecord curRec=query.record();//获得一条空数据
    curRec.setValue("EmpNo",3000+qryModel->rowCount());

    //1.打开子页面，传入一个空的记录
    TDialogData *dataDialog=new TDialogData(this);
    dataDialog->setRecord(curRec);//通过将curRec传入setRecord函数，设置数据
    if(dataDialog->exec()==QDialog::Accepted)
    {
        //2.在子页面中设置数据，返回到主页面
        QSqlRecord recData=dataDialog->record();//通过record函数获得子页面传回的设置的数据
        //3.利用QSqlQuery对象的prepare函数，准备sql语句，准备往数据库中插入数据
        query.prepare("INSERT INTO employee (EmpNo,Name,Gender,Birthday,Province,"
                      " Department,Salary,Memo,Photo)"
                      " VALUES(:EmpNo,:Name,:Gender,:Birthday,:Province,"
                      " :Department,:Salary,:Memo,:Photo)"
                      );
        //4.通过bindValue函数绑定数据，设置sql语句中的参数，来源于recData里面的值
        query.bindValue(":EmpNo",recData.value("EmpNo"));
        query.bindValue(":Name",recData.value("Name"));
        query.bindValue(":Gender",recData.value("Gender"));
        query.bindValue(":Birthday",recData.value("Birthday"));
        query.bindValue(":Province",recData.value("Province"));
        query.bindValue(":Department",recData.value("Department"));
        query.bindValue(":Salary",recData.value("Salary"));
        query.bindValue(":Memo",recData.value("Memo"));
        query.bindValue(":Photo",recData.value("Photo"));

        //5.执行sql语句，插入数据
        if(!query.exec())
            QMessageBox::critical(this,"error","插入数据错误\n"+query.lastError().text());
        else
        {
            //6.前面都是在数据库里面更新数据，这里也在model里面更新数据,从而显示在view里面
            qryModel->setQuery(qryModel->query().executedQuery());//执行sql语句，更新数据
        }
    }
    delete dataDialog;
}

//编辑记录
void MainWindow::on_actRecEdit_triggered()
{
    //获得当前选中的行号，通过QSelectModel对象的currentIndex函数获得当前选中的行号
    int curRecNo=selModel->currentIndex().row();
    updateRecord(curRecNo);//传入当前记录，然后进行更新数据
}

//双击记录
void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    updateRecord(index.row());
}

//删除记录
void MainWindow::on_actRecDelete_triggered()
{
    int curRecNo=selModel->currentIndex().row();
    QSqlRecord curRec=qryModel->record(curRecNo);
    if(curRec.isEmpty())
        return;

    int empNo=curRec.value("EmpNo").toInt();
    QSqlQuery query;
    query.prepare("delete from employee where EmpNo=:ID");
    query.bindValue(":ID",empNo);

    if(!query.exec())
        QMessageBox::critical(this,"error","删除失败\n"+query.lastError().text());
    else{
      //  qryModel->query().exec();//这个不会完全更新model
      //前面都是数据库里面进行删除数据，这里也在model里面删除数据,从而显示在view里面
        qryModel->setQuery(qryModel->query().lastQuery());
    }
}

//涨工资
void MainWindow::on_actScan_triggered()
{
    QSqlQuery qryUpdate;
    qryUpdate.exec("UPDATE employee SET Salary=Salary+1000");
   //qryModel->query().exec();
    qryModel->setQuery(qryModel->query().lastQuery());
}

