#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
void MainWindow::openTable()
{
    //1.创建模型，打开数据库表格
    tabModel=new QSqlTableModel(this,DB);
    //设置model的各种属性
    tabModel->setTable("employee");//设置表格名
    tabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置编辑策略为手动提交，即数据库的数据不自动提交（这里的逻辑是model和数据库是双向的，后面遇到修改model时候，需要手动提交一下）
    tabModel->setSort(tabModel->fieldIndex("empNo"),Qt::AscendingOrder);//设置默认排序字段为工号升序
    //model->select()方法会执行SQL语句，打开表格
    if(!(tabModel->select()))
    {
        QMessageBox::critical(this,"错误","数据库表格打开失败："+tabModel->lastError().text());
    }
    //2.表格model成功打开后的设置view逻辑，设置状态栏显示记录条数
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));

    //设置view字段显示的标题
    tabModel->setHeaderData(tabModel->fieldIndex("empNo"),  Qt::Horizontal, "工号");
    tabModel->setHeaderData(tabModel->fieldIndex("Name"),   Qt::Horizontal, "姓名");
    tabModel->setHeaderData(tabModel->fieldIndex("Gender"), Qt::Horizontal, "性别");
    tabModel->setHeaderData(tabModel->fieldIndex("Birthday"),   Qt::Horizontal, "出生日期");
    tabModel->setHeaderData(tabModel->fieldIndex("Province"),   Qt::Horizontal, "省份");
    tabModel->setHeaderData(tabModel->fieldIndex("Department"), Qt::Horizontal, "部门");
    tabModel->setHeaderData(tabModel->fieldIndex("Salary"), Qt::Horizontal, "工资");
    //tabModel->setHeaderData(tabModel->fieldIndex("Memo"),Qt::Horizontal,"备注");
    //tabModel->setHeaderData(tabModel->fieldIndex("Photo"),Qt::Horizontal,"照片");

    //3.model/view的关联
    selModel=new QItemSelectionModel(tabModel,this);

    ui->tableView->setModel(tabModel);
    ui->tableView->setSelectionModel(selModel);
    //4.隐藏不需要显示的字段
    ui->tableView->setColumnHidden(tabModel->fieldIndex("Memo"),true);
    ui->tableView->setColumnHidden(tabModel->fieldIndex("Photo"),true);

    //5.代理与view指定字段的关联
    QStringList strList;
    strList<<"男"<<"女";
    //设置下拉列表的内容，并且设置为不可编辑
    delegateSex.setItems(strList,false);
    //设置代理关联到指定的字段
    ui->tableView->setItemDelegateForColumn(
                tabModel->fieldIndex("Gender"),&delegateSex);

    strList.clear();
    strList<<"技术部"<<"销售部"<<"市场部";
    delegateDepart.setItems(strList,false);
    //设置代理关联到指定的字段
    ui->tableView->setItemDelegateForColumn(
                tabModel->fieldIndex("Department"),&delegateDepart);

    //6.model字段与widget映射，即数据库->model->widget的映射，在完成映射之后，还需要具体的动作槽函数来实现数据的提交和回滚，来实现具体的逻辑
    dataMapper=new QDataWidgetMapper(this);
    dataMapper->setModel(tabModel);//设置model
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    dataMapper->addMapping(ui->dbSpinEmpNo,tabModel->fieldIndex("embpNo"));
    dataMapper->addMapping(ui->dbEditName,tabModel->fieldIndex("Name"));
    dataMapper->addMapping(ui->dbComboSex,tabModel->fieldIndex("Gender"));
    dataMapper->addMapping(ui->dbEditBirth,tabModel->fieldIndex("Birthday"));
    dataMapper->addMapping(ui->dbComboProvince,tabModel->fieldIndex("Province"));
    dataMapper->addMapping(ui->dbComboDep,tabModel->fieldIndex("Department"));
    dataMapper->addMapping(ui->dbSpinSalary,tabModel->fieldIndex("Salary"));
    dataMapper->addMapping(ui->dbEditMemo,tabModel->fieldIndex("Memo"));
    dataMapper->toFirst();//设置到第一条记录

    //7.加载完数据库后action的状态存在是否可编辑的问题
    ui->actOpenDB->setEnabled(false);
    ui->actRecAppend->setEnabled(true);
    ui->actRecAppend->setEnabled(true);
    ui->actRecInsert->setEnabled(true);
    ui->actRecDelete->setEnabled(true);
    ui->actScan->setEnabled(true);

    ui->groupBoxSort->setEnabled(true);
    ui->groupBoxFilter->setEnabled(true);

    //8.获取字段名更新combobox
    QSqlRecord emptyRec=tabModel->record();//获取空记录
    for(int i=0;i<emptyRec.count();++i)
        ui->comboFields->addItem(emptyRec.fieldName(i));//将字段名添加到下拉列表中

    //9.来实现第6步的映射后，当前行改变时，更新widget的内容，其实这相当于view的两个组件的信号槽连接，信号都是QItemSelectionModel的信号
    connect(selModel,&QItemSelectionModel::currentChanged,this,&MainWindow::do_currentChanged);
    connect(selModel,&QItemSelectionModel::currentRowChanged,this,&MainWindow::do_currentRowChanged);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setCentralWidget(ui->splitter);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//单选
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

    DB=QSqlDatabase::addDatabase("QSQLITE"); //添加驱动
    DB.setDatabaseName(aFile);//设置数据库文件名
    if(!DB.open())
        QMessageBox::warning(this,"错误","数据库打开失败");
    else
        openTable(); //打开数据库表格
}
//槽函数do_currentChanged和do_currentRowChanged的作用是：当选中行改变时，更新widget的内容
void MainWindow::do_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);Q_UNUSED(previous);
    ui->actSubmit->setEnabled(tabModel->isDirty());//判断是否有修改,如果有修改则提交按钮可用
    ui->actRevert->setEnabled(tabModel->isDirty());//判断是否有修改,如果有修改则回滚按钮可用，即取消修改
}

//当选中行改变时，更新widget的内容
void MainWindow::do_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if(!current.isValid())
    {
        ui->dbLabPhoto->clear();
        return;
    }

    //设置各种action的可用状态
    ui->actRecDelete->setEnabled(true);
    ui->actPhoto->setEnabled(true);
    ui->actPhotoClear->setEnabled(true);

    //映射到widget
    dataMapper->setCurrentIndex(current.row());
    //更新照片显示
    QSqlRecord curRec=tabModel->record(current.row());
    if(curRec.isNull("Photo"))
        ui->dbLabPhoto->clear();
    else
    {
        QPixmap pic;
        pic.loadFromData(curRec.value("Photo").toByteArray());//加载图片数据
        ui->dbLabPhoto->setPixmap(pic.scaledToWidth(ui->dbLabPhoto->size().width()));//设置图片大小，显示
    }
}


//前面所有的逻辑都是在打开数据库表格后，设置model和view的逻辑，即顺序是数据库->model->view->widget的映射关系
//下面是前面view的改变，涉及到model的改变，model的改变涉及到数据库的改变，即顺序是view->model->数据库的改变，这是通过QSqlRecord来实现的,通过它在QModel中获取当前行的记录，修改记录，然后设置model的当前行的记录为修改后的记录，最后提交到数据库。QModel和数据库是双向的
//添加记录，涉及model增加，selectionmodel的修改选中项，以及状态栏的更新
void MainWindow::on_actRecAppend_triggered()
{
    QSqlRecord rec=tabModel->record();//获取空记录
    rec.setValue(tabModel->fieldIndex("Gender"),"男");//设置model新增一行里面的性别字段的值为男
    tabModel->insertRecord(tabModel->rowCount(),rec);//在model最后一行插入一条记录

    selModel->clearSelection();//清除selectionmodel的选中状态
    selModel->setCurrentIndex(tabModel->index(tabModel->rowCount()-1,1),
                              QItemSelectionModel::Select);//设置selectionmodel的当前行为最后一行
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));
}

//插入记录，涉及model增加，selectionmodel的修改选中项，以及状态栏的更新
void MainWindow::on_actRecInsert_triggered()
{
    QModelIndex curIndex=ui->tableView->currentIndex();//获取当前选中行，即要插入的位置
    QSqlRecord rec=tabModel->record();
    rec.setValue(tabModel->fieldIndex("Gender"),"男");
    tabModel->insertRecord(curIndex.row(),rec);

    selModel->clearSelection();
    selModel->setCurrentIndex(curIndex,
                              QItemSelectionModel::Select);
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));
}

//删除记录，涉及model删除，selectionmodel的修改选中项，以及状态栏的更新
void MainWindow::on_actRecDelete_triggered()
{
    QModelIndex curIndex=ui->tableView->currentIndex();
    tabModel->removeRow(curIndex.row());
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));
}

//添加照片，涉及model的修改，selectionmodel的修改选中项，以及状态栏的更新
void MainWindow::on_actPhoto_triggered()
{
    QString aFile=QFileDialog::getOpenFileName(this,"选择一个图片","","招聘(*.jpg)");
    if(aFile.isEmpty())
        return;

    QFile *file=new QFile(aFile);
    if(file->open(QIODevice::ReadOnly))
    {
        QByteArray data=file->readAll();
        file->close();
        delete file;//删除QFile对象

        QSqlRecord curRec=tabModel->record(selModel->currentIndex().row());//获取当前行的记录，记录是一个QSqlRecord对象
        curRec.setValue("Photo",data);//设置当前行的Photo字段的值为图片数据
        tabModel->setRecord(selModel->currentIndex().row(),curRec);//设置model的当前行的记录为curRec

        QPixmap pix;
        pix.load(aFile);
        ui->dbLabPhoto->setPixmap(pix.scaledToWidth(ui->dbLabPhoto->width()));
    }
}

//清除照片，涉及model的修改，selectionmodel的修改选中项，以及状态栏的更新
void MainWindow::on_actPhotoClear_triggered()
{
    QSqlRecord curRec=tabModel->record(selModel->currentIndex().row());
    curRec.setNull("Photo");
    tabModel->setRecord(selModel->currentIndex().row(),curRec);
    ui->dbLabPhoto->clear();
}

//涨工资，涉及model的修改，selectionmodel的修改选中项，以及状态栏的更新
void MainWindow::on_actScan_triggered()
{
    if(tabModel->rowCount()==0)
        return;

    for(int i=0;i<tabModel->rowCount();++i)
    {
        QSqlRecord aRec=tabModel->record(i);//获取当前行的记录，记录是一个QSqlRecord对象
        aRec.setValue("Salary",aRec.value("Salary").toFloat()*1.1);//设置当前行的Salary字段的值为原来的1.1倍
        tabModel->setRecord(i,aRec);
    }

    if(tabModel->submitAll())
        QMessageBox::information(this,"消息","涨工资执行完毕！");
}

//提交修改，涉及model的提交，selectionmodel的修改选中项，以及状态栏的更新
void MainWindow::on_actSubmit_triggered()
{
    bool res=tabModel->submitAll();
    if(!res)
        QMessageBox::information(this,"消息","数据库保存失败\n"+tabModel->lastError().text());
    else{
        //设置action的可用状态为不可用
        ui->actSubmit->setEnabled(false);
        ui->actRevert->setEnabled(false);
    }
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));
}

//回滚修改，涉及model的回滚，selectionmodel的修改选中项，以及状态栏的更新
void MainWindow::on_actRevert_triggered()
{
    tabModel->revertAll();
    ui->actSubmit->setEnabled(false);
    ui->actRevert->setEnabled(false);
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));
}

//排序，涉及model的排序，selectionmodel的修改选中项，以及状态栏的更新，这里用的是model的排序函数setSort，setSort函数会自动调用select函数来执行SQL语句，打开表格，进行排序
void MainWindow::on_comboFields_currentIndexChanged(int index)
{
    if(ui->radioBtnAscend->isChecked())
        tabModel->setSort(index,Qt::AscendingOrder);
    else
        tabModel->setSort(index,Qt::DescendingOrder);
    //排完序后，需要重新选择一下，才能显示出来，即重新完成model-view的显示
    tabModel->select();
}

//升序，这是不涉及model的排序，单纯view的排序,用的函数是sort
void MainWindow::on_radioBtnAscend_clicked()
{
    tabModel->sort(ui->comboFields->currentIndex(),Qt::AscendingOrder);
}

//降序
void MainWindow::on_radioBtnDescend_clicked()
{
    tabModel->sort(ui->comboFields->currentIndex(),Qt::DescendingOrder);
}

//性别过滤
void MainWindow::on_radioBtnMan_clicked()
{
    tabModel->setFilter("Gender='男'");
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));
}

//性别过滤
void MainWindow::on_radioBtnWoman_clicked()
{
    tabModel->setFilter("Gender='女'");
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));
}
//性别过滤的全显示
void MainWindow::on_radioBtnBoth_clicked()
{
    tabModel->setFilter("");
    ui->statusBar->showMessage(QString("记录条数为：%1").arg(tabModel->rowCount()));
}

