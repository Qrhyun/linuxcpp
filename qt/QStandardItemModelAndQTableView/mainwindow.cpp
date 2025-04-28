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

    /*
    从文件内容中获取第一行，这通常包含了列标题。
    使用正则表达式 R"(\s+)" 按一个或多个空白字符分割标题行，得到列标题列表 headerList。
    使用 m_model->setHorizontalHeaderLabels(headerList) 设置模型的水平表头标签。
    */
    QString header=aFileContnet.at(0);
    QStringList headerList=header.split(QRegularExpression(R"(\s+)"),Qt::SkipEmptyParts);
    m_model->setHorizontalHeaderLabels(headerList);

    /*
    从第二行开始遍历文件内容（因为第一行是标题）。
    对于每一行，使用相同的正则表达式分割字符串，得到列数据 tmpList。
    对于除了最后一列之外的每一列，创建一个新的 QStandardItem 对象，并将其设置到模型中对应的位置。
    对于最后一列，创建一个新的 QStandardItem 对象，将其设置为可检查（setCheckable(true)），并设置背景颜色为黄色（setBackground(QBrush(Qt::yellow))）。
    根据当前列的数据设置检查状态（setCheckState），如果数据是 "0"，则设置为未选中（Qt::Unchecked），否则设置为选中（Qt::Checked）。
    将创建的项 aItem 设置到模型中对应的位置。
    */
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
    QString curPath=QCoreApplication::applicationDirPath();//获取当前应用程序的路径
    QString aFileName=QFileDialog::getOpenFileName(this,"打开一个文件",curPath,
                                                   "数据文件(*.txt);;所有文件(*.*)");
    /*
    弹出一个文件对话框，让用户选择要打开的文件。
    this 参数指定对话框的父窗口。
    第二个参数是对话框的标题。
    第三个参数是对话框的起始目录，这里使用应用程序的路径。
    最后一个参数是文件过滤器，指定对话框中显示哪些类型的文件。
    */
    if(aFileName.isEmpty())
        return;

    //打开文件
    /*
    创建一个 QFile 对象 aFile，其构造函数接收文件名。
    尝试以只读和文本模式打开文件。如果文件无法打开（例如，因为权限问题或文件不存在），函数将返回
    */
    QFile aFile(aFileName);
    if(!aFile.open(QIODevice::ReadOnly|QIODevice::Text))
        return;

    /*
    读取文件内容
    创建一个空的 QStringList 对象 aFileContent，用于存储文件的每一行。
    清空 plainTextEdit 控件的内容，准备显示新文件的内容。
    创建一个 QTextStream 对象 aStream，用于从文件中读取文本。
    使用 while 循环和 readLine() 方法逐行读取文件内容，直到文件结束。
    将每一行内容追加到 plainTextEdit 控件中显示，并添加到 aFileContent 列表中。
    关闭文件。
    */
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

    /*
    更新界面
    更新状态栏中的 labCurFile 标签，显示当前打开的文件名。
    启用一系列动作（如追加、插入、保存、删除），这些动作现在可以对当前打开的文件执行。
    */
    labCurFile->setText("当前文件："+aFileName);
    ui->actAppend->setEnabled(true);
    ui->actInsert->setEnabled(true);
    ui->actSave->setEnabled(true);
    ui->actDelete->setEnabled(true);

    //调用 iniModelData() 函数，传入 aFileContent 列表，初始化模型数据。
    iniModelData(aFileContent);
}

//将 QStandardItemModel（m_model）中的数据导出到 QPlainTextEdit 控件中显示
void MainWindow::on_actModelData_triggered()
{
    //1.清空文本编辑器的内容
    ui->plainTextEdit->clear();
    QStandardItem *aItem;
    QString str;

    //2.导出表头数据
    for(int i=0;i<m_model->columnCount();++i)
    {
        //遍历模型的列数，获取每一列的表头项（horizontalHeaderItem）
        aItem=m_model->horizontalHeaderItem(i);
        //将表头项的文本追加到 str 中，并在每个表头后面添加一个制表符（"\t"）作为分隔
        str+=aItem->text();
        str+="\t";
    }
    //将表头项的文本追加到文本编辑器中显示
    ui->plainTextEdit->appendPlainText(str);

    //3.导出表格数据行
    for(int i=0;i<m_model->rowCount();++i)
    {
        //对于每一行，首先清空 str 字符串
        str="";
        //遍历每一列（除了最后一列），获取当前列的项（item(i, j)）。
        for(int j=0;j<m_model->columnCount()-1;++j)
        {
            aItem=m_model->item(i,j);
            //将项的文本追加到 str 中，并在每个数据后面添加一个制表符（"\t"）作为分隔
            str+=aItem->text();
            str+="\t";
        }
        //获取最后一列的项，检查其选中状态（checkState()）。根据选中状态，将 "是" 或 "否" 追加到 str 中。
        aItem=m_model->item(i,m_model->columnCount()-1);
        if(aItem->checkState()==Qt::Checked)
            str+="是";
        else
            str+="否";
        //将整行数据追加到文本编辑器中显示
        ui->plainTextEdit->appendPlainText(str);
    }
}



void MainWindow::on_actAppend_triggered()
{
    //1.创建一个空的 QList<QStandardItem*> 列表，用于存储新创建的数据项。
    QList<QStandardItem*> aItemList;//这个定义在for循环外面是下面要用好几次
    //遍历模型的列数（减去最后一列），为除了最后一列之外的每一列创建一个新的 QStandardItem 对象，并将其初始化为 "0"
    QStandardItem *aItem;
    //2.创建除了最后一列之外的每一列的数据项
    for(int i=0;i<m_model->columnCount()-1;++i)
    {
        aItem=new QStandardItem("0");
        //将每个新创建的数据项添加到 aItemList 列表中。
        aItemList<<aItem;
    }
    //3.创建最后一列的数据项
    //获取模型的最后一列的表头名称，这里假设表头数据存储在模型中
    QString str=m_model->headerData(m_model->columnCount()-1,Qt::Horizontal).toString();
    aItem=new QStandardItem(str);
    //设置 aItem 为可检查（setCheckable(true)），使其成为一个复选框项。
    aItem->setCheckable(true);
    aItem->setCheckable(true);
    aItem->setBackground(QBrush(Qt::yellow));
    //将 aItem 添加到 aItemList 列表中。
    aItemList<<aItem;
    //4.插入新行到模型,使用 insertRow 方法在模型的末尾插入一行新数据
    m_model->insertRow(m_model->rowCount(),aItemList);
    //5.最后，函数使新插入的行成为当前选中的行。
    //首先清除所有选中的项（clearSelection）。然后设置当前选中的项为新插入行的第一列（setCurrentIndex）。这里假设新插入的行是模型的最后一行。
    m_selection->clearSelection();
    m_selection->setCurrentIndex(m_model->index(m_model->rowCount()-1,0),
                                 QItemSelectionModel::Select);

}


void MainWindow::on_actInsert_triggered()
{
    QList<QStandardItem*> aItemList;
    QStandardItem *aItem;//这个定义在for循环外面是下面要用好几次
    //遍历模型的列数（减去最后一列），为除了最后一列之外的每一列创建一个新的 QStandardItem 对象，并将其初始化为 "0"
    for(int i=0;i<m_model->columnCount()-1;++i)
    {
        aItem=new QStandardItem("0");
        //将每个新创建的数据项添加到 aItemList 列表中。
        aItemList<<aItem;
    }
    //获取模型的最后一列的表头名称，这里假设表头数据存储在模型中。
    QString str=m_model->headerData(m_model->columnCount()-1,Qt::Horizontal).toString();
    aItem=new QStandardItem(str);
    //设置 aItem 为可检查（setCheckable(true)），使其成为一个复选框项。
    aItem->setCheckable(true);
    aItem->setBackground(QBrush(Qt::yellow));
    //将 aItem 添加到 aItemList 列表中。
    aItemList<<aItem;
    QModelIndex curIndex=m_selection->currentIndex();
    m_model->insertRow(curIndex.row(),aItemList);
    m_selection->clearSelection();
    m_selection->setCurrentIndex(curIndex, QItemSelectionModel::Select);
}


void MainWindow::on_actDelete_triggered()
{
    //1.获取当前选中的索引
    QModelIndex curIndex=m_selection->currentIndex();

    if(curIndex.row()!=m_model->rowCount()-1)
    {
        //如果当前选中的行不是最后一行，调用 m_model->removeRow(curIndex.row()) 从模型中删除该行。
        m_model->removeRow(curIndex.row());
        //然后调用 m_selection->setCurrentIndex(curIndex, QItemSelectionModel::Select) 重新设置当前选中的项为刚刚删除行的前一行。这样做是为了保证用户界面的一致性，避免因为删除操作导致选中项失效
        m_selection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
    }else{
        //如果当前选中的行是最后一行，直接调用 m_model->removeRow(curIndex.row()) 删除该行。在这种情况下，不需要重新设置选中项，因为删除最后一行后，用户界面会自动更新。
        m_model->removeRow(curIndex.row());
    }
}

//这个函数的作用是将表格模型中当前选中的项（QStandardItem）的文本对齐方式设置为左对齐并垂直居中
void MainWindow::on_actAlignLeft_triggered()
{
    if(!m_selection->hasSelection())
        return;

    //selectedIndexes() 方法返回一个包含所有选中项索引的列表（QModelIndexList）
    QModelIndexList indexList=m_selection->selectedIndexes();
    for(auto aIndex:indexList)
    {
        m_model->itemFromIndex(aIndex)->setTextAlignment(
                    Qt::AlignLeft|Qt::AlignVCenter);
    }
}

//这个函数的作用是将表格模型中当前选中的项（QStandardItem）的文本对齐方式设置为居中对齐并垂直居中
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

//这个函数的作用是将表格模型中当前选中的项（QStandardItem）的文本对齐方式设置为右对齐并垂直居中
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

//这个函数的作用是将表格模型中当前选中的项（QStandardItem）的字体设置为加粗或取消加粗
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

