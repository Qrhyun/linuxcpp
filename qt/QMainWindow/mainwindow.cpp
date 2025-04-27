#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QActionGroup>
#include <QSpinBox>
#include <QFontComboBox>
#include <QLabel>
#include <QProgressBar>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //解决英文界面，中文界面互斥
    ui->setupUi(this);
    //QActionGroup对象是一个QAction对象的集合，提供了对一组QAction对象的管理功能，其的addAction()方法可以将QAction对象添加到集合中，setExclusive()方法可以设置该集合是否互斥。
    QActionGroup *actionGroup=new QActionGroup(this);
    actionGroup->addAction(ui->actLang_CN);
    actionGroup->addAction(ui->actLang_EN);
    actionGroup->setExclusive(true);

    //创建一些无法通过可视化UI直接设计的组件]

    //一.在一个工具栏（QToolBar）中动态添加一个 QSpinBox 控件和一个 QFontComboBox 控件，用于设置字体大小和字体类型。
    //1.QSpinBox是一个专门用于选择字体大小的下拉列表控件。其的setMinimum()和setMaximum()方法可以设置输入的字体的最小值和最大值（用于调整 QTextEdit 控件的字体大小），setValue()方法可以设置当前值，setMinimumWidth()方法可以设置控件的最小宽度。
    QSpinBox *spinFontSize=new QSpinBox(this);
    spinFontSize->setMinimum(5);
    spinFontSize->setMaximum(50);
    spinFontSize->setValue(ui->textEdit->font().pointSize());//ui->textEdit->font().pointSize()：获取 QTextEdit 控件当前字体的点大小，并将其设置为 QSpinBox 控件的当前值。
    spinFontSize->setMinimumWidth(50);
    ui->toolBar->addWidget(spinFontSize);

    //2.QFontComboBox 是一个专门用于选择字体样式（即宋体，黑体啥的）的下拉列表控件。其的setMinimumWidth()方法可以设置控件的最小宽度，setFont()方法可以设置当前字体（即 QTextEdit 控件的字体）。
    QFontComboBox *comboFontBox=new QFontComboBox(this);
    comboFontBox->setMinimumWidth(150);
    comboFontBox->setFont(ui->textEdit->font());//ui->textEdit->font()：获取 QTextEdit 控件当前字体，并将其设置为 QFontComboBox 控件的当前字体。
    ui->toolBar->addWidget(comboFontBox);

    //3.QToolBar 是一个工具栏控件，可以在其中添加一些操作按钮（QAction 对象）。其的addAction()方法可以将 QAction 对象添加到工具栏中，addSeparator()方法可以在工具栏中添加分隔符。
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actClose);

    //二.在状态栏QStatusBar中动态添加一个 QLabel 控件和一个 QProgressBar 控件，用于显示当前文件名和字体大小的进度条。
    labFile=new QLabel(this);
    labFile->setMinimumWidth(150);
    labFile->setText("文件名：");
    ui->statusbar->addWidget(labFile);
    progressbar=new QProgressBar(this);
    progressbar->setMinimum(5);
    progressbar->setMaximum(50);
    progressbar->setValue(ui->textEdit->font().pointSize());
    ui->statusbar->addWidget(progressbar);
    labInfo=new QLabel("Permanent");
    ui->statusbar->addPermanentWidget(labInfo);

    connect(spinFontSize,&QSpinBox::valueChanged,this,&MainWindow::on_fontSize_changed);
    connect(comboFontBox,&QFontComboBox::currentFontChanged,this,&MainWindow::on_font_changed);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actFile_New_triggered()
{
    ui->textEdit->clear();
    ui->textEdit->document()->setModified(false);
    labFile->setText("文件名：新建的一个文件");
}


void MainWindow::on_actFile_Open_triggered()
{
    labFile->setText("正在打开一个文件");
}


void MainWindow::on_actFile_Save_triggered()
{
    ui->textEdit->document()->setModified(false);
    labFile->setText("文件以保存");
}


void MainWindow::on_textEdit_copyAvailable(bool b)
{
    ui->actEdit_Copy->setEnabled(b);
    ui->actEdit_Cut->setEnabled(b);
    ui->actEdit_Paste->setEnabled(ui->textEdit->canPaste());
}


void MainWindow::on_textEdit_selectionChanged()
{
    QTextCharFormat fmt=ui->textEdit->currentCharFormat();
    ui->actFont_Bold->setChecked(fmt.font().bold());
    ui->actFont_Italic->setChecked(fmt.font().italic());
    ui->actFont_UnderLine->setChecked(fmt.font().underline());
}


void MainWindow::on_actFont_Bold_triggered(bool checked)
{
    QTextCharFormat fmt=ui->textEdit->currentCharFormat();
    if(checked)
        fmt.setFontWeight(QFont::Bold);
    else
        fmt.setFontWeight(QFont::Normal);
    ui->textEdit->setCurrentCharFormat(fmt);
}


void MainWindow::on_actFont_Italic_toggled(bool arg1)
{
    QTextCharFormat fmt=ui->textEdit->currentCharFormat();
    fmt.setFontItalic(arg1);
    ui->textEdit->setCurrentCharFormat(fmt);
}


void MainWindow::on_actFont_UnderLine_triggered(bool checked)
{
    QTextCharFormat fmt=ui->textEdit->currentCharFormat();
    fmt.setFontUnderline(checked);
    ui->textEdit->setCurrentCharFormat(fmt);
}

void MainWindow::on_fontSize_changed(int fontSize)
{
    QTextCharFormat fmt=ui->textEdit->currentCharFormat();
    fmt.setFontPointSize(fontSize);
    ui->textEdit->setCurrentCharFormat(fmt);
    progressbar->setValue(fontSize);
}

void MainWindow::on_font_changed(const QFont &font)
{
    QTextCharFormat fmt=ui->textEdit->currentCharFormat();
    fmt.setFont(font);
    ui->textEdit->setCurrentCharFormat(fmt);
    labInfo->setText("字体名称："+font.family());
}

