#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_model=new QFileSystemModel(this);
    ui->treeView->setModel(m_model);
    ui->listView->setModel(m_model);
    ui->tableView->setModel(m_model);

   // ui->treeView->setRootIndex(m_model->index(QDir::currentPath()));//作用是将treeView的根节点设置为当前工作目录（QDir::currentPath()）对应的索引
   //影响范围：仅影响treeView，不会影响其他视图（如listView、tableView）。
    m_model->setRootPath("");

    connect(ui->treeView,&QTreeView::clicked,ui->listView,&QListView::setRootIndex);
    connect(ui->treeView,&QTreeView::clicked,ui->tableView,&QTableView::setRootIndex);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//功能是设置根目录，允许用户选择一个目录作为根目录，并更新三个视图（treeView、listView、tableView）的根索引为所选目录的索引。
void MainWindow::on_actSetRoot_triggered()
{
    QString dir=QFileDialog::getExistingDirectory(this,"选择目录",QDir::currentPath());
    if(dir.isEmpty())
        return;

    ui->treeView->setRootIndex(m_model->index(dir));
    ui->listView->setRootIndex(m_model->index(dir));
    ui->tableView->setRootIndex(m_model->index(dir));
}

//显示目录和文件
void MainWindow::on_radioShowAll_clicked()
{
    //显示目录和文件时候，对于文件过滤的各项功能（QGroupBox）设置为可用
    ui->groupBoxFilter->setEnabled(true);
    //用model的setFilter函数设置过滤器，QDir::AllDirs表示显示所有目录，QDir::Files表示显示所有文件，QDir::NoDotAndDotDot表示不显示“.”和“..”这两个特殊目录。
    m_model->setFilter(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
}

//显示目录
void MainWindow::on_radioShowOnlyDir_clicked()
{
    ui->groupBoxFilter->setEnabled(false);
    m_model->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
}

//用于实现文件过滤功能的启用或禁用
void MainWindow::on_chkBoxEnableFilter_clicked(bool checked)
{
    ui->comboFilters->setEnabled(checked);//启用或禁用下拉列表comboFilters
    ui->btnApplyFilters->setEnabled(checked);//同样启用或禁用按钮btnApplyFilters
    m_model->setNameFilterDisables(!checked);//调用QFileSystemModel的setNameFilterDisables方法，设置文件过滤是否启用
}

//过滤
void MainWindow::on_btnApplyFilters_clicked()
{
    QStringList filter=ui->comboFilters->currentText().trimmed().split(";",Qt::SkipEmptyParts);
    m_model->setNameFilters(filter);
}

//显示文件名、路径、类型和大小等信息
void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    //获取当前选中的索引，index是QModelIndex类型的对象，表示树视图中选中的项的索引。
    //通过QFileSystemModel的fileName、filePath、type和size方法获取文件名、路径、类型和大小等信息，并将这些信息显示在相应的标签上。
    ui->labFileName->setText(m_model->fileName(index));
    ui->labPath->setText(m_model->filePath(index));
    ui->labType->setText(m_model->type(index));

    ui->chkIsDir->setEnabled(m_model->isDir(index));

    int sz=m_model->size(index)/1024;
    if(sz<1024)
        ui->labFileSize->setText(QString("%1 KB").arg(sz));
    else
        ui->labFileSize->setText(QString::asprintf("%.1f MB",sz/1024.0));
}

