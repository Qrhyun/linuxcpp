#include "tformdoc.h"
#include "ui_tformdoc.h"
#include<QFile>
#include <QFileInfo>
TFormDoc::TFormDoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TFormDoc)
{
    ui->setupUi(this);
    setWindowTitle("New Doc[*]");//设置窗口标题
    setAttribute(Qt::WA_DeleteOnClose);//设置窗口关闭时删除对象
    connect(ui->plainTextEdit,&QPlainTextEdit::modificationChanged,
            this,&TFormDoc::setWindowModified);//当文本编辑器内容发生变化时，设置窗口修改状态为true，也就是设置窗口标题为[*]
}

TFormDoc::~TFormDoc()
{
    delete ui;
}
//加载文件
{
    QFile aFile(aFileName);
    if(aFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream aStream(&aFile);
        ui->plainTextEdit->clear();
        ui->plainTextEdit->setPlainText(aStream.readAll());
        aFile.close();

        QFileInfo fileInfo(aFileName);//获取文件信息，包括文件名、大小、创建时间等
        QString str=fileInfo.fileName();//获取文件名
        setWindowTitle(str+"[*]");  //设置窗口标题
        m_fileName=str;//aFileName
        m_fileOpened=true;
    }
}
//当前文件名
QString TFormDoc::currentFileName()
{
    return m_fileName;
}
//是否打开文件
bool TFormDoc::isFileOpened()
{
    return m_fileOpened;
}
//保存文件，与上面的setWindowTitle()函数配合使用,对应占位符[*]的消失
void TFormDoc::saveToFile()
{
    setWindowModified(false);//设置窗口未修改，也就是设置窗口标题为不带[*]的文件名
}
#include <QFontDialog>
//设置字体
void TFormDoc::setEditFont()
{
    QFont font;
    font=ui->plainTextEdit->font();
    bool ok;
    font=QFontDialog::getFont(&ok,font);
    if(ok)
        ui->plainTextEdit->setFont(font);
}
//剪切
void TFormDoc::textCut()
{
    ui->plainTextEdit->cut();
}
//复制
void TFormDoc::textCopy()
{
    ui->plainTextEdit->copy();
}
//粘贴
void TFormDoc::textPaste()
{
    ui->plainTextEdit->paste();
}
//上面都是分页面的功能，但是对应的action按钮在mainwindow中，所以这里面手动实现下action的功能，然后在mainwindow中调用