#include "tformdoc.h"
#include "ui_tformdoc.h"
#include <QToolBar>
#include <QVBoxLayout>
#include <QFontDialog>

TFormDoc::TFormDoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TFormDoc)
{
    ui->setupUi(this);
    //代码实现工具栏
    //设置工具栏
    QToolBar *locToolBar=new QToolBar("文档",this);
    //添加一些动作到工具栏
    locToolBar->addAction(ui->actOpen);
    locToolBar->addAction(ui->actFont);
    locToolBar->addSeparator();
    locToolBar->addAction(ui->actCut);
    locToolBar->addAction(ui->actCopy);
    locToolBar->addAction(ui->actPaste);
    locToolBar->addAction(ui->actRedo);
    locToolBar->addAction(ui->actUndo);
    locToolBar->addSeparator();
    locToolBar->addAction(ui->actClose);
    //设置风格为图标在上，文字在下
    locToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //使用QVBoxLayout布局,这是一个垂直布局
    QVBoxLayout *layout=new QVBoxLayout();
    layout->addWidget(locToolBar);
    layout->addWidget(ui->plainTextEdit);
    //this是当前窗口的指针，利用setLayout函数设置布局
    this->setLayout(layout);
}

TFormDoc::~TFormDoc()
{
    delete ui;
}
#include<QFileDialog>
#include<QFileInfo>
//嵌入式窗口的打开文件函数
void TFormDoc::on_actOpen_triggered()
{
    QString curPath=QCoreApplication::applicationDirPath();
    QString aFileName=QFileDialog::getOpenFileName(this,"打开一个文件",curPath,
            "C程序文件(*.h *.cpp);;文本文件(*.txt);;所有文件(*.*)");

    if(aFileName.isEmpty())
        return;

    QFile aFile(aFileName);
    if(aFile.open(QIODevice::ReadOnly|QIODevice::Text))//以只读文本的方式打开文件
    {
        ui->plainTextEdit->clear();
        QTextStream aStream(&aFile);//创建文本流对象
        while(!aStream.atEnd())
        {
            QString str=aStream.readLine();//读取一行文本
            ui->plainTextEdit->appendPlainText(str);
        }
        aFile.close();

        QFileInfo fileInfo(aFileName);
        QString shortName=fileInfo.fileName();
        setWindowTitle(shortName);//只能在独立窗口中使用setWindowTitle函数，即在独立Wight中使用
        emit titleChanged(shortName);//也能在嵌入式窗口使用connect信号和槽函数,交给原对象系统处理
    }
}

//嵌入式窗口的字体设置函数
void TFormDoc::on_actFont_triggered()
{
    QFont font=ui->plainTextEdit->font();
    bool ok;//获取字体
    //QFontDialog::getFont(&ok,font)函数返回一个bool值，ok为true表示用户选择了字体
    font = QFontDialog::getFont(&ok,font);//参数里面的font是原来的字体，最后返回的font是用户选择的字体
    if(ok)
        ui->plainTextEdit->setFont(font);
}

