#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tformdoc.h"
#include <QMdiSubWindow>
#include <QMdiSubWindow>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->mdiArea);//设置中心窗口为mdiArea
}

MainWindow::~MainWindow()
{
    delete ui;
}

//新建文档
void MainWindow::on_actDoc_New_triggered()
{
    TFormDoc *formDoc=new TFormDoc();
    ui->mdiArea->addSubWindow(formDoc);//midiArea添加子窗口
    formDoc->show();//显示子窗口

    //设置各项action可用
    ui->actCut->setEnabled(true);
    ui->actCopy->setEnabled(true);
    ui->actPaste->setEnabled(true);
    ui->actFont->setEnabled(true);
}

//打开文档.调用子页面的isFileOpened()函数判断是否打开文件
void MainWindow::on_actDoc_Open_triggered()
{
    bool needNew=true;
    TFormDoc *formDoc;
    if(ui->mdiArea->subWindowList().size()>0)
    {
        formDoc=dynamic_cast<TFormDoc*>(ui->mdiArea->activeSubWindow()->widget());
        needNew=formDoc->isFileOpened();
    }

    //需要新建文档的逻辑
    //1.首先打开文件对话框，获取文件名
    QString curPath=QCoreApplication::applicationDirPath();
    QString aFileName=QFileDialog::getOpenFileName(this,"打开一个文件",curPath,
                    "C程序文件(*.h *.cpp);;文本文件(*.txt);;所有文件(*.*)");
    if(aFileName.isEmpty())
        return;
    //2.新建一个子页面，并加在入mdiArea中
    if(needNew)
    {
        formDoc=new TFormDoc(this);
        ui->mdiArea->addSubWindow(formDoc);
    }
    //3.调用子页面中的loadFromFile()函数，加载新打开的文件到子页面中，并显示出来
    formDoc->loadFromFile(aFileName);
    formDoc->show();
    //4.设置各项action可用，当然旧的子页面已经打开了，所以这里是为新打开的子页面设置的，和新建文档的逻辑一样
    ui->actCut->setEnabled(true);
    ui->actCopy->setEnabled(true);
    ui->actPaste->setEnabled(true);
    ui->actFont->setEnabled(true);
}

//设置字体，调用子页面的setEditFont()函数
void MainWindow::on_actFont_triggered()
{
    TFormDoc *formDoc=dynamic_cast<TFormDoc*>(ui->mdiArea->activeSubWindow()->widget());
    formDoc->setEditFont();
}

//剪切，调用子页面的textCut()函数
void MainWindow::on_actCut_triggered()
{
    TFormDoc *formDoc=dynamic_cast<TFormDoc*>(ui->mdiArea->activeSubWindow()->widget());
    formDoc->textCut();
}

//复制，调用子页面的textCopy()函数
void MainWindow::on_actCopy_triggered()
{
    TFormDoc *formDoc=dynamic_cast<TFormDoc*>(ui->mdiArea->activeSubWindow()->widget());
    formDoc->textCopy();
}

//粘贴，调用子页面的textPaste()函数
void MainWindow::on_actPaste_triggered()
{
    TFormDoc *formDoc=dynamic_cast<TFormDoc*>(ui->mdiArea->activeSubWindow()->widget());
    formDoc->textPaste();
}

//设置标签式视图或子窗口式视图
void MainWindow::on_actViewMode_triggered(bool checked)
{
    if(checked)
        //设置为标签式视图
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    else
        //设置为子窗口式视图
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);

    //下面三个分别设置mdiArea的标签可关闭、级联和平铺的action是否可用
    ui->mdiArea->setTabsClosable(checked);
    ui->actCascade->setEnabled(!checked);
    ui->actTile->setEnabled(!checked);
}

//级联子窗口
void MainWindow::on_actCascade_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

//平铺子窗口
void MainWindow::on_actTile_triggered()
{
    ui->mdiArea->tileSubWindows();
}

//关闭所有子窗口
void MainWindow::on_actCloseALL_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

//保存文件，调用子页面的saveToFile()函数
void MainWindow::on_actDoc_Save_triggered()
{
    TFormDoc *formDoc=dynamic_cast<TFormDoc*>(ui->mdiArea->activeSubWindow()->widget());
    formDoc->saveToFile();
}

//当没有子窗口时，设置各项action不可用，设置mdiArea的标签不可关闭
void MainWindow::on_mdiArea_subWindowActivated(QMdiSubWindow *arg1)
{
    Q_UNUSED(arg1);
    if(ui->mdiArea->subWindowList().size()==0)
    {
        ui->actCut->setEnabled(false);
        ui->actCopy->setEnabled(false);
        ui->actPaste->setEnabled(false);
        ui->actFont->setEnabled(false);
    }
}

