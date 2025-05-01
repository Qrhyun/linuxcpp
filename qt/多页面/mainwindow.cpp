#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tformdoc.h"
#include <QPainter>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //初始化QTabWidget
    setCentralWidget(ui->tabWidget);//设置中心窗口为tabWidget
    ui->tabWidget->setVisible(false);//设置tabWidget不可见
    ui->tabWidget->clear();//清空tabWidget
    ui->tabWidget->setTabsClosable(true);//设置tabWidget的标签页可关闭,这里对应下面的on_tabWidget_tabCloseRequested(int index)函数
    //setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//嵌入式窗口QWidget的实现 用于在 QTabWidget 中动态添加一个嵌入式窗口（TFormDoc 类型的对象）
void MainWindow::on_actWidgetInsite_triggered()
{
    TFormDoc *formDoc=new TFormDoc(this);
    formDoc->setAttribute(Qt::WA_DeleteOnClose);//设置属性为关闭时删除
    //将 formDoc 添加为 QTabWidget 的一个新标签页，并设置标签标题为 Doc X，其中 X 是当前标签页的数量（count() 返回标签页数量）
    int cur=ui->tabWidget->addTab(formDoc,
                          QString::asprintf("Doc %d",ui->tabWidget->count()));
    //设置当前活动的标签页为刚刚添加的标签页
    ui->tabWidget->setCurrentIndex(cur);
    //设置标签页可见
    ui->tabWidget->setVisible(true);
    connect(formDoc,&TFormDoc::titleChanged,this,&MainWindow::do_changeTabTitle);
}

void MainWindow::do_changeTabTitle(QString str)
{
    int index=ui->tabWidget->currentIndex();
    ui->tabWidget->setTabText(index,str);
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->widget(index)->close();
}
//重写paintEvent函数，实现窗口背景图片的绘制
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //
    painter.drawPixmap(0,ui->mainToolBar->height(),
                       width(),height()-ui->mainToolBar->height()-ui->statusBar->height(),
                       QPixmap(":/images/images/back2.jpg"));
}

//独立的窗口QWidget的实现，这里仅仅是显示一个窗口，和上面的嵌入式窗口不同，不嵌入到QTabWidget中
void MainWindow::on_actWidget_triggered()
{
    TFormDoc *formDoc=new TFormDoc();//没有parent
    formDoc->setAttribute(Qt::WA_DeleteOnClose);//设置属性为关闭时删除
    formDoc->setWindowTitle("基于QWidget的窗口，无parent");
    formDoc->setWindowOpacity(0.8);//设置窗口透明度为0.8
    formDoc->show();//显示窗口
}

#include <tformtable.h>

void MainWindow::on_actWindowInsite_triggered()
{
    TFormTable *formTable=new TFormTable(this);
    formTable->setAttribute(Qt::WA_DeleteOnClose);
    int cur=ui->tabWidget->addTab(formTable,
                          QString::asprintf("Table %d",ui->tabWidget->count()));
    ui->tabWidget->setCurrentIndex(cur);
    ui->tabWidget->setVisible(true);
}


void MainWindow::on_actWindow_triggered()
{
    TFormTable *formTable=new TFormTable(this);
    formTable->setAttribute(Qt::WA_DeleteOnClose);
    formTable->setWindowTitle("独立的window");
    formTable->show();
}

