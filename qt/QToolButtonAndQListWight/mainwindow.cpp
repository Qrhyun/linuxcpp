#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QToolButton按钮可以通过设置setDefaultAction()函数，可以与Action关联
    ui->tBtnListIni->setDefaultAction(ui->actListIni);
    ui->tBtnListClear->setDefaultAction(ui->actListClear);
    ui->tBtnListInsert->setDefaultAction(ui->actListInsert);
    ui->tBtnListAppend->setDefaultAction(ui->actListAppend);
    ui->tBtnListDelete->setDefaultAction(ui->actListDelete);

    ui->tBtnSelALL->setDefaultAction(ui->actSelAll);
    ui->tBtnSelNone->setDefaultAction(ui->actSelNone);
    ui->tBtnSelInvs->setDefaultAction(ui->actSelInvs);

    //QMenu是一个菜单类，可以在菜单栏中显示菜单，QMenu的addAction()方法可以添加菜单项，exec()方法可以执行菜单
    QMenu* menuSelection=new QMenu(this);
    menuSelection->addAction(ui->actSelAll);
    menuSelection->addAction(ui->actSelNone);
    menuSelection->addAction(ui->actSelInvs);

    //QMenu的setPopupMode()方法可以设置菜单的弹出模式，QToolButton的setPopupMode()方法可以设置按钮的弹出模式，QToolButton的setMenu()方法可以设置按钮的菜单
    ui->tBtnSelectItem->setPopupMode(QToolButton::MenuButtonPopup);
    ui->tBtnSelectItem->setMenu(menuSelection);

    //这里创建了一个QToolButton按钮，并设置了弹出模式为InstantPopup，设置了文本和图标，并设置了菜单为menuSelection，最后将按钮添加到工具栏toolBox中
    QToolButton *aBtn=new QToolButton(this);
    aBtn->setPopupMode(QToolButton::InstantPopup);
    aBtn->setText("选择选项");
    aBtn->setIcon(QIcon(":/images/icons/424.bmp"));
    aBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    aBtn->setMenu(menuSelection);
    ui->toolBar->addWidget(aBtn);

    //这里添加了一个分隔符，添加了一个退出按钮，并设置了图标和文本，最后将按钮添加到工具栏toolBar中
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actQuit);
}

MainWindow::~MainWindow()
{
    delete ui;
}
// QListWidget是一个列表控件，可以显示一组项目，用户可以选择其中的一个或多个项目。QListWidgetItem是QListWidget的一个元素，类似于QTreeWidgetItem
void MainWindow::on_actListIni_triggered()
{
    QListWidgetItem *aItem;
    ui->listWidget->clear();//列表清除内容
    QIcon aIcon;
    aIcon.addFile(":/images/icons/check2.ico");
    bool chk=ui->chkListEditable->isChecked();
    for(int i=0;i<10;i++)
    {   //QListWidgetItem是QListWidget的一个元素，类似于QTreeWidgetItem，里面的setText()方法可以设置显示的文本,setIcon()方法可以设置图标,setCheckState()方法可以设置选中状态
        aItem=new QListWidgetItem();
        aItem->setText("Item"+QString::number(i));
        aItem->setIcon(aIcon);
        aItem->setCheckState(Qt::Checked);
        //setFlags()方法可以设置元素的属性，Qt::ItemIsEditable表示该元素可以被编辑，Qt::ItemIsSelectable表示该元素可以被选中，Qt::ItemIsUserCheckable表示该元素可以被用户选中，Qt::ItemIsEnabled表示该元素是可用的
        if(chk)
            aItem->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        else
            aItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        ui->listWidget->addItem(aItem);
    }
}

// QListWidget的清除，使用clear()方法清除所有元素，使用takeItem()方法删除当前行的元素，使用delete()方法删除当前行的元素
void MainWindow::on_actListClear_triggered()
{
    ui->listWidget->clear();
}

// QListWidget的插入，使用insertItem()方法在当前行插入一个新的元素（插入是插入到光标选择的位置insert），使用takeItem()方法删除当前行的元素
void MainWindow::on_actListInsert_triggered()
{
    QIcon aIcon(":/images/icons/check2.ico");
    bool chk=ui->chkListEditable->isChecked();

    QListWidgetItem *aItem=new QListWidgetItem();
    aItem->setText("Item Inserted");
    aItem->setIcon(aIcon);
    aItem->setCheckState(Qt::Checked);

    if(chk)
        aItem->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    else
        aItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    ui->listWidget->insertItem(ui->listWidget->currentRow(),aItem);//在当前行插入一个新的元素

}

// QListWidget的追加，使用addItem()方法在当前行插入一个新的元素（添加是添加到最后push_back），使用takeItem()方法删除当前行的元素
void MainWindow::on_actListAppend_triggered()
{
    QIcon aIcon(":/images/icons/check2.ico");
    bool chk=ui->chkListEditable->isChecked();
    QListWidgetItem * aItem=new QListWidgetItem();
    aItem->setText("Item added");
    aItem->setIcon(aIcon);
    aItem->setCheckState(Qt::Checked);

    if(chk)
        aItem->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    else
        aItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    ui->listWidget->addItem(aItem);
}

// QListWidget的删除，使用takeItem()方法删除当前行的元素，使用delete()方法删除当前行的元素
void MainWindow::on_actListDelete_triggered()
{
    int row = ui->listWidget->currentRow();
    delete ui->listWidget->takeItem(row);
}

// QListWidget的全部选中，使用setCheckState()方法设置选中状态
void MainWindow::on_actSelAll_triggered()
{
    int cnt=ui->listWidget->count();
    for(int i=0;i<cnt;++i)
    {
        QListWidgetItem *aItem=ui->listWidget->item(i);
        aItem->setCheckState(Qt::Checked);
    }
}

// QListWidget的全部不选中，使用setCheckState()方法设置选中状态
void MainWindow::on_actSelNone_triggered()
{
    int cnt=ui->listWidget->count();
    for(int i=0;i<cnt;++i)
    {
        QListWidgetItem *aItem=ui->listWidget->item(i);
        aItem->setCheckState(Qt::Unchecked);
    }
}

// QListWidget的反选，使用setCheckState()方法设置选中状态
void MainWindow::on_actSelInvs_triggered()
{
    int cnt=ui->listWidget->count();
    for(int i=0;i<cnt;++i)
    {
        QListWidgetItem *aItem=ui->listWidget->item(i);
        if(aItem->checkState()==Qt::Checked)
            aItem->setCheckState(Qt::Unchecked);
        else
            aItem->setCheckState(Qt::Checked);
    }
}

//响应 QListWidget 的 currentItemChanged 信号，当用户在列表中选择不同的项时，更新界面中的文本框和文本编辑区域
void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    QString str;
    if(current!=NULL)
    {
        if(previous==NULL)
            str="当前项："+current->text();
         else
            str="前一项："+previous->text()+";当前项："+current->text();

        ui->editCutItemText->setText(str);
    }

    ui->plainTextEdit->appendPlainText("currentItemChanged()信号被发射");
}


void MainWindow::on_chkBoxSorting_clicked(bool checked)
{
    ui->listWidget->setSortingEnabled(checked);
}


void MainWindow::on_tBtnSortAsc_clicked()
{
    ui->listWidget->sortItems(Qt::AscendingOrder);
}


void MainWindow::on_toolButton_2_clicked()
{
    ui->listWidget->sortItems(Qt::DescendingOrder);
}


void MainWindow::on_tBtnClearText_clicked()
{
    ui->plainTextEdit->clear();
}


void MainWindow::on_tBtnAddLine_clicked()
{
    ui->plainTextEdit->appendPlainText("");
}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->plainTextEdit->appendPlainText("itemDoubleClicked:"+item->text());
}


void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu * menuList=new QMenu();
    menuList->addAction(ui->actListIni);
    menuList->addAction(ui->actListClear);
    menuList->addAction(ui->actListInsert);
    menuList->addAction(ui->actListAppend);
    menuList->addAction(ui->actListDelete);
    menuList->addSeparator();
    menuList->addAction(ui->actSelAll);
    menuList->addAction(ui->actSelNone);
    menuList->addAction(ui->actSelInvs);
    menuList->exec(QCursor::pos());
    delete menuList;
}


