#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include<QStandardItemModel>
#include<QItemSelectionModel>
#include <QFileDialog>
void MainWindow::iniModelData(QStringList &aFileContnet)
{
    int rowCnt=aFileContnet.size();
    m_model->setRowCount(rowCnt-1);//第一行是标题

    QString header=aFileContnet.at(0);
    QStringList headerList=header.split(QRegularExpression(R"(\s+)"),Qt::SkipEmptyParts);
    m_model->setHorizontalHeaderLabels(headerList);

    QStandardItem *aItem;
    int j;
    for(int i=1;i<rowCnt;++i)
    {
        QString aLineText=aFileContnet.at(i);
        QStringList tmpList=aLineText.split(QRegularExpression(R"(\s+)"),Qt::SkipEmptyParts);
        for(j=0;j<FixedColumnCount-1;++j)
        {
            aItem=new QStandardItem(tmpList.at(j));
            m_model->setItem(i-1,j,aItem);
        }
        aItem=new QStandardItem(headerList.at(j));
        aItem->setCheckable(true);
        aItem->setBackground(QBrush(Qt::yellow));
        if(tmpList.at(j)=="0")
            aItem->setCheckState(Qt::Unchecked);
        else
            aItem->setCheckState(Qt::Checked);
        m_model->setItem(i-1,j,aItem);
    }
}

void MainWindow::do_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    if(current.isValid()){
        labCellPos->setText(QString::asprintf("当前单元格：%d行，%d列",
                                              current.row(),current.column()));
        //获取当前单元格的内容，注意这里是通过QStandardItemModel里面的itemFromIndex项来获取的
        QStandardItem *aItem=m_model->itemFromIndex(current);//QStandardItemModel::itemFromIndex()函数通过当前索引返回当前单元格的QStandardItem项
        labCellText->setText("单元内容："+aItem->text());
        ui->actFontBold->setChecked(aItem->font().bold());//aItem->font().bold()的功能是获取当前单元格的字体font()是否加粗bold(),返回true或false，action的checked属性是bool类型的，所以可以直接赋值
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->splitter);//设置主窗口的中心部件为分割器，区别于之前splitter单独设置为一层的用法，这里直接设置为主窗口的中心部件，和其他的部件一样并列

    //继续完成声明的控件的初始化，并挂到对象树this上
    labCurFile=new QLabel("当前文件",this);
    labCurFile->setMinimumWidth(200);
    labCellPos=new QLabel("当前单元格",this);
    labCellPos->setMinimumWidth(200);
    labCellText=new QLabel("单元格内容",this);
    labCellText->setMinimumWidth(200);
    //加到状态栏上
    ui->statusbar->addWidget(labCurFile);
    ui->statusbar->addWidget(labCellPos);
    ui->statusbar->addWidget(labCellText);
    //初始化
    m_model=new QStandardItemModel(2,FixedColumnCount,this);
    m_selection=new QItemSelectionModel(m_model,this);//与QStandardItemModel模型关联
    //设置表格的模型和选择模型，即让tableView和模型关联起来
    ui->tableView->setModel(m_model);
    ui->tableView->setSelectionModel(m_selection);
    //设置表格的选择模式和选择行为，分别是ExtendedSelection和SelectItems，即允许多选和选择单元格
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

    connect(m_selection,&QItemSelectionModel::currentChanged,
            this,&MainWindow::do_currentChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actOpen_triggered()
{
    QString curPath=QCoreApplication::applicationDirPath();
    QString aFileName=QFileDialog::getOpenFileName(this,"打开一个文件",curPath,
                                                   "数据文件(*.txt);;所有文件(*.*)");
    if(aFileName.isEmpty())
        return;

    QFile aFile(aFileName);
    if(!aFile.open(QIODevice::ReadOnly|QIODevice::Text))
        return;

    QStringList aFileContent;
    ui->plainTextEdit->clear();
    QTextStream aStream(&aFile);
    while(!aStream.atEnd())
    {
        QString str=aStream.readLine();
        ui->plainTextEdit->appendPlainText(str);
        aFileContent.append(str);
    }
    aFile.close();

    labCurFile->setText("当前文件："+aFileName);
    ui->actAppend->setEnabled(true);
    ui->actInsert->setEnabled(true);
    ui->actSave->setEnabled(true);
    ui->actDelete->setEnabled(true);

    iniModelData(aFileContent);
}


void MainWindow::on_actModelData_triggered()
{
    ui->plainTextEdit->clear();
    QStandardItem *aItem;
    QString str;

    for(int i=0;i<m_model->columnCount();++i)
    {
        aItem=m_model->horizontalHeaderItem(i);
        str+=aItem->text();
        str+="\t";
    }
    ui->plainTextEdit->appendPlainText(str);

    for(int i=0;i<m_model->rowCount();++i)
    {
        str="";
        for(int j=0;j<m_model->columnCount()-1;++j)
        {
            aItem=m_model->item(i,j);
            str+=aItem->text();
            str+="\t";
        }
        aItem=m_model->item(i,m_model->columnCount()-1);
        if(aItem->checkState()==Qt::Checked)
            str+="是";
        else
            str+="否";

        ui->plainTextEdit->appendPlainText(str);
    }
}



void MainWindow::on_actAppend_triggered()
{
    QList<QStandardItem*> aItemList;
    QStandardItem *aItem;
    for(int i=0;i<m_model->columnCount()-1;++i)
    {
        aItem=new QStandardItem("0");
        aItemList<<aItem;
    }
    QString str=m_model->headerData(m_model->columnCount()-1,Qt::Horizontal).toString();
    aItem=new QStandardItem(str);
    aItem->setCheckable(true);
    aItem->setBackground(QBrush(Qt::yellow));
    aItemList<<aItem;
    m_model->insertRow(m_model->rowCount(),aItemList);
    m_selection->clearSelection();
    m_selection->setCurrentIndex(m_model->index(m_model->rowCount()-1,0),
                                 QItemSelectionModel::Select);

}


void MainWindow::on_actInsert_triggered()
{
    QList<QStandardItem*> aItemList;
    QStandardItem *aItem;
    for(int i=0;i<m_model->columnCount()-1;++i)
    {
        aItem=new QStandardItem("0");
        aItemList<<aItem;
    }
    QString str=m_model->headerData(m_model->columnCount()-1,Qt::Horizontal).toString();
    aItem=new QStandardItem(str);
    aItem->setCheckable(true);
    aItem->setBackground(QBrush(Qt::yellow));
    aItemList<<aItem;
    QModelIndex curIndex=m_selection->currentIndex();
    m_model->insertRow(curIndex.row(),aItemList);
    m_selection->clearSelection();
    m_selection->setCurrentIndex(curIndex, QItemSelectionModel::Select);
}


void MainWindow::on_actDelete_triggered()
{
    QModelIndex curIndex=m_selection->currentIndex();

    if(curIndex.row()!=m_model->rowCount()-1)
    {
        m_model->removeRow(curIndex.row());
        m_selection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
    }else{
        m_model->removeRow(curIndex.row());
    }
}


void MainWindow::on_actAlignLeft_triggered()
{
    if(!m_selection->hasSelection())
        return;

    QModelIndexList indexList=m_selection->selectedIndexes();
    for(auto aIndex:indexList)
    {
        m_model->itemFromIndex(aIndex)->setTextAlignment(
                    Qt::AlignLeft|Qt::AlignVCenter);
    }
}


void MainWindow::on_actAlignCenter_triggered()
{
    if(!m_selection->hasSelection())
        return;

    QModelIndexList indexList=m_selection->selectedIndexes();
    for(auto aIndex:indexList)
    {
        m_model->itemFromIndex(aIndex)->setTextAlignment(
                    Qt::AlignHCenter|Qt::AlignVCenter);
    }
}


void MainWindow::on_actAlignRight_triggered()
{
    if(!m_selection->hasSelection())
        return;

    QModelIndexList indexList=m_selection->selectedIndexes();
    for(auto aIndex:indexList)
    {
        m_model->itemFromIndex(aIndex)->setTextAlignment(
                    Qt::AlignRight|Qt::AlignVCenter);
    }
}


void MainWindow::on_actFontBold_triggered(bool checked)
{
    if(!m_selection->hasSelection())
        return;

    QModelIndexList indexList=m_selection->selectedIndexes();
    for(auto aIndex:indexList)
    {
        QFont font=m_model->itemFromIndex(aIndex)->font();
        font.setBold(checked);
        m_model->itemFromIndex(aIndex)->setFont(font);
    }
}

