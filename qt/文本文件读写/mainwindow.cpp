#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSaveFile>
#include <QException>
#include <QMessageBox>

·

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actOpen_IODevice_triggered()
{
    //用只读的方式打开一个文件
    QFile aFile(getFile());//根据上面的文件路径
    if(!aFile.exists())
        return;
    if(!aFile.open(QIODevice::ReadOnly|QIODevice::Text))
        return;
    ui->textEditDevice->appendPlainText(aFile.readAll());
    aFile.close();
}

//文件另存为
void MainWindow::on_actSave_IODevice_triggered()
{
    QFile aFile(getFile(true));
    if(!aFile.open(QIODevice::WriteOnly|QIODevice::Text))
        return;
    QString str=ui->textEditDevice->toPlainText();//获取文本框的内容
    aFile.write(str.toUtf8(),str.toUtf8().length());//将文本框的内容转换为utf-8格式的字节流

    aFile.close();
}
//文件另存为
void MainWindow::on_actSave_TextSafe_triggered()
{
    QSaveFile aFile(getFile(true));
    if(!aFile.open(QIODevice::WriteOnly|QIODevice::Text))//尝试以“只写”和“文本模式”打开文件
        return;

    aFile.setDirectWriteFallback(false);//设置 QSaveFile 的直接写入回退功能为 false 这意味着QSaveFile 不会直接写入原文件，而是先写入一个临时文件，成功写入后才会commit到原文件
    try{
        QString str=ui->textEditDevice->toPlainText();
        aFile.write(str.toUtf8(),str.toUtf8().length());//调用 aFile.write 将字节流写入文件
        aFile.commit();//用 aFile.commit() 提交写入操作，确保数据保存到文件中
    }
    catch(QException &e)
    {
        QMessageBox::warning(this,"异常","保存文件过程方式错误！");
        aFile.cancelWriting();//调用 aFile.cancelWriting() 取消写入操作，确保文件不会被破坏
    }
}

//以文本流的方式打开文件
void MainWindow::on_actOpen_TextStream_triggered()
{
    QFile aFile(getFile());
    if(!aFile.exists())
        return;
    if(!aFile.open(QIODevice::ReadOnly|QIODevice::Text))
        return;
    QTextStream aStream(&aFile);//创建一个文本流对象，传入文件对象
    aStream.setAutoDetectUnicode(true);//设置自动检测文本是否为Unicode编码
    while(!aStream.atEnd())
    {
        QString str=aStream.readLine();//读取一行文本
        ui->textEditStream->appendPlainText(str);//将读取的文本添加到文本编辑器中
    }
    aFile.close();
}

#include <QTextBlock>
//以文本流的方式保存文件
void MainWindow::on_actSave_TextStream_triggered()
{
    QSaveFile aFile(getFile(true));
    if(!aFile.open(QIODevice::WriteOnly|QIODevice::Text))
        return;

    aFile.setDirectWriteFallback(false);
    try{
        QTextStream aStream(&aFile);
        aStream.setAutoDetectUnicode(true);
        QTextDocument *doc=ui->textEditStream->document();//获取文本编辑器的文档对象，获取用户界面中 textEditStream 文本编辑器的文档对象 doc，用于逐行读取文本内容
        for(int i=0;i<doc->blockCount();++i)
        {
            aStream<<doc->findBlockByNumber(i).text();//遍历文档中的每一段（block），通过 findBlockByNumber(i) 获取第 i 段的文本内容
            aStream<<"\n";//使用 aStream 将每段文本写入文件，并在每段后添加换行符 \n
        }
        aFile.commit();
    }
    catch(QException &e)
    {
        QMessageBox::warning(this,"异常","保存文件过程方式错误！");
        aFile.cancelWriting();
    }
}

