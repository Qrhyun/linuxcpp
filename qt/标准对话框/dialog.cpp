#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QProgressDialog>
#include <QElapsedTimer>
#include <QInputDialog>
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

//QFileDialog的getOpenFileName()函数可以选择一个文件
void Dialog::on_btnOpen_clicked()
{
    QString curPath=QCoreApplication::applicationDirPath();
    QString fileName=QFileDialog::getOpenFileName(this,"选择一个文件",curPath,
                                 "文本文件(*.txt);;图片(*.jpg *.gif *png);;所有文件(*.*)");

    if(!fileName.isEmpty())
        ui->plainTextEdit->appendPlainText(fileName);
}

// QFileDialog的getOpenFileNames()函数可以选择多个文件
void Dialog::on_btnOpenMulti_clicked()
{
    QString curPath=QCoreApplication::applicationDirPath();
    QStringList fileNames=QFileDialog::getOpenFileNames(this,"选择多个文件",curPath,
                                 "文本文件(*.txt);;图片(*.jpg *.gif *png);;所有文件(*.*)");

    for(auto fileName:fileNames)
        ui->plainTextEdit->appendPlainText(fileName);
}

// QFileDialog的getExistingDirectory()函数可以选择一个目录
void Dialog::on_btnSelDir_clicked()
{
     QString curPath=QCoreApplication::applicationDirPath();
     QString selectedDir=QFileDialog::getExistingDirectory(this,"选择一个目录",curPath);
     if(!selectedDir.isEmpty())
         ui->plainTextEdit->appendPlainText(selectedDir);
}

// QFileDialog的getSaveFileName()函数可以选择一个文件保存路径
void Dialog::on_btnSave_clicked()
{
    QString curPath=QCoreApplication::applicationDirPath();
    QString fileName=QFileDialog::getSaveFileName(this,"保存文件",curPath,
                            "文本文件(*.txt);;图片(*.jpg *.gif *png);;所有文件(*.*)");

    if(!fileName.isEmpty())
        ui->plainTextEdit->appendPlainText(fileName);
}

// QColorDialog的getColor()函数可以选择一个颜色
void Dialog::on_btnColor_clicked()
{
    QPalette pal=ui->plainTextEdit->palette();//获取当前调色板
    QColor iniColor=pal.color(QPalette::Text);//获取当前文本颜色
    QColor color=QColorDialog::getColor(iniColor,this,"选择颜色");
    if(color.isValid())
    {
        pal.setColor(QPalette::Text,color);//设置文本颜色
        ui->plainTextEdit->setPalette(pal);//设置调色板
    }
}

// QFontDialog的getFont()函数可以选择一个字体
void Dialog::on_btnFont_clicked()
{
    QFont iniFont=ui->plainTextEdit->font();//获取当前字体
    bool ok=false;
    QFont font=QFontDialog::getFont(&ok,iniFont);//获取字体
    if(ok)
        ui->plainTextEdit->setFont(font);
}

// QProgressDialog的setValue()函数可以设置进度条的值
void Dialog::on_btnProgress_clicked()
{
    int minValue=0;
    int maxValue=200;
    QProgressDialog dlgProgress("正在复制文件...","取消",minValue,maxValue,this);//创建进度条
    dlgProgress.setWindowTitle("复制文件");//设置标题
    dlgProgress.setWindowModality(Qt::WindowModal);//设置模态，这里的模态是指在进度条弹出时，不能操作其他窗口，通过函数setWindowModality()来设置
    connect(&dlgProgress,&QProgressDialog::canceled,[&]{ui->plainTextEdit->appendPlainText("进度已取消！");});
    QElapsedTimer msCounter;//因为复制一下就完成了，所以我们用计时器来模拟一下复制的过程
    for(int i=minValue;i<=maxValue;++i)
    {
        dlgProgress.setValue(i);//设置进度条的值
        dlgProgress.setLabelText(QString::asprintf("正在复制文件，第%d",i));//设置进度条的文本

        msCounter.start();
        while(true)
        {
            if(msCounter.elapsed()>30)
                break;
        }

        if(dlgProgress.wasCanceled())//判断是否被取消
            break;
    }


}

// QinputDialog的getText()函数可以输入一个字符串
void Dialog::on_btnInputString_clicked()
{
    QString dlgTitle="输入文字对话框";
    QString txtLabel="请输入文件名";
    QString iniInput="新建文件.txt";
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;
    bool ok=false;
    QString text=QInputDialog::getText(this,dlgTitle,txtLabel,echoMode,iniInput,&ok);
    if(ok&&!text.isEmpty())
        ui->plainTextEdit->appendPlainText(text);
}

// QInputDialog的getInt()函数可以输入一个整数
void Dialog::on_btnInputInt_clicked()
{
    QString dlgTitle="输入整数对话框";
    QString txtLabel="设置文本框字体大小";
    int defaultValue=ui->plainTextEdit->font().pointSize();
    int minValue=6;
    int maxValue=60;
    int stepValue=1;
    bool ok=false;

    int inputValue=QInputDialog::getInt(this,dlgTitle,txtLabel,defaultValue,
                                        minValue,maxValue,stepValue,&ok);

    if(ok)
    {
        QString str=QString("文本框字体大小被设置为：%1").arg(inputValue);//QString::arg()函数可以格式化字符串
        ui->plainTextEdit->appendPlainText(str);
        QFont font=ui->plainTextEdit->font();
        font.setPointSize(inputValue);
        ui->plainTextEdit->setFont(font);
    }
}


void Dialog::on_btnInputFloat_clicked()
{
    QString dlgTitle="输入浮点数对话框";
    QString txtLabel="输入一个浮点数";
    float defaultValue=3.13;

    float minValue=0;
    float maxValue=10000;
    int decimals=2;

    bool ok=false;
    float inputValue = QInputDialog::getDouble(this, dlgTitle,txtLabel,
                                               defaultValue, minValue,maxValue,decimals,&ok);
    if (ok)
    {
        QString str=QString::asprintf("输入了一个浮点数:%.2f",inputValue);
        ui->plainTextEdit->appendPlainText(str);
    }
}

// QInputDialog的getItem()函数可以输入一个字符串列表
void Dialog::on_btnInputItem_clicked()
{
    QStringList items;
    items<<"优"<<"良"<<"中"<<"差";
    QString dlgTitle="输入项对话框";
    QString txtLabel="请选择一个级别";
    int curIndex=0;
    bool editable=false;
    bool ok=false;//返回值

    QString text=QInputDialog::getItem(this,dlgTitle,txtLabel,items,curIndex,
                                       editable,&ok);

    if(ok&&!text.isEmpty())
        ui->plainTextEdit->appendPlainText(text);
}
#include<QMessageBox>

// QMessageBox的question()函数可以输入一个问题
void Dialog::on_btnMsgQuestion_clicked()
{
    QString dlgTitle="Question消息框";
    QString strInfo="文件已被修改，是否保存?";

    int result=QMessageBox::question(this,dlgTitle,strInfo,
                          QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
                          QMessageBox::NoButton);

    if(result==(int)QMessageBox::Yes)
        ui->plainTextEdit->appendPlainText("Question消息框：Yes被选中");
    else  if(result==QMessageBox::No)
        ui->plainTextEdit->appendPlainText("Question消息框：No被选中");
    else  if(result==QMessageBox::Cancel)
        ui->plainTextEdit->appendPlainText("Question消息框：Cancel被选中");
    else
        ui->plainTextEdit->appendPlainText("Question消息框：没有按钮被选中");
}

// QMessageBox的information()函数可以输入一个提示信息
void Dialog::on_btnMsgInformation_clicked()
{
    QString dlgTitle="Information消息框";
    QString strInfo="文件已经打开，请检查！";

    QMessageBox::information(this,dlgTitle,strInfo);
}

// QMessageBox的warning()函数可以输入一个警告信息
void Dialog::on_btnMsgWarning_clicked()
{
    QString dlgTitle="Warning消息框";
    QString strInfo="文件已被修改！";

    QMessageBox::warning(this,dlgTitle,strInfo);
}

// QMessageBox的critical()函数可以输入一个错误信息
void Dialog::on_btnMsgCritical_clicked()
{
    QString dlgTitle="Critical消息框";
    QString strInfo="文件已被修改！！！！！";

    QMessageBox::critical(this,dlgTitle,strInfo);
}

// QMessageBox的about()函数可以输入一个关于信息
// QMessageBox的aboutQt()函数可以输入一个关于Qt的信息
void Dialog::on_btnMsgAbout_clicked()
{
    QString dlgTitle="About消息框";
    QString strInfo="XXXX版本1.0！";

    QMessageBox::about(this,dlgTitle,strInfo);
}

// QMessageBox的aboutQt()函数可以输入一个关于Qt的信息
void Dialog::on_btnMsgAboutQt_clicked()
{
    QString dlgTile="aboutQt消息框";
    QMessageBox::aboutQt(this,dlgTile);
}

