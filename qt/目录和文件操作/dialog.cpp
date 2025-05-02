#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QTemporaryDir>
#include <QTemporaryFile>

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

// QCoreApplication::applicationDirPath()  当前应用程序所在的目录,QFileDialog::getOpenFileName()  打开文件对话框
void Dialog::on_pushButton_41_clicked()
{
    QString curDir=QCoreApplication::applicationDirPath();
    QString aFile=QFileDialog::getOpenFileName(this,"打开一个文件",curDir,"所有文件(*.*)");
    ui->editFile->setText(aFile);
}

// QFileDialog::getExistingDirectory()  打开目录对话框
void Dialog::on_pushButton_45_clicked()
{
    QString curDir=QCoreApplication::applicationDirPath();
    QString aDir=QFileDialog::getExistingDirectory(this,"选择一个目录",curDir);
    ui->editDir->setText(aDir);
}


void Dialog::on_pushButton_5_clicked()
{
    ui->plainTextEdit->clear();
}


void Dialog::on_pushButton_clicked()
{
    showBtnInfo(sender());
    QString str=QCoreApplication::applicationDirPath();
    ui->plainTextEdit->appendPlainText(str);
}

// 显示按钮的名称和提示信息
void Dialog::showBtnInfo(QObject *btn)
{
    QPushButton *theBtn=static_cast<QPushButton*>(btn);//将QObject转换为QPushButton
    ui->plainTextEdit->appendPlainText(theBtn->text());//按钮的名称
    ui->plainTextEdit->appendPlainText(theBtn->toolTip()+":\n");//按钮的提示信息
}

// QCoreApplication::applicationFilePath()  当前应用程序的完整路径
void Dialog::on_pushButton_2_clicked()
{
    showBtnInfo(sender());
    QString str=QCoreApplication::applicationFilePath();
    ui->plainTextEdit->appendPlainText(str);
}

// QCoreApplication::applicationName()  当前应用程序的名称
void Dialog::on_pushButton_3_clicked()
{
    showBtnInfo(sender());
   // QCoreApplication::setApplicationName("MyApp");
    QString str=QCoreApplication::applicationName();
    ui->plainTextEdit->appendPlainText(str);
}

// QCoreApplication::applicationName()  当前应用程序的名称
void Dialog::on_pushButton_62_clicked()
{
    showBtnInfo(sender());
    QCoreApplication::setApplicationName("MyApp");
    QString str=QCoreApplication::applicationName();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QCoreApplication::libraryPaths()  当前应用程序的库路径
void Dialog::on_pushButton_4_clicked()
{
    showBtnInfo(sender());
    QStringList strList=QCoreApplication::libraryPaths();
    for(auto &item:strList)
        ui->plainTextEdit->appendPlainText(item);

    ui->plainTextEdit->appendPlainText("");
}

// QCoreApplication::origanizationName()  当前应用程序的组织名称,setOrganizationName()  设置当前应用程序的组织名称
void Dialog::on_pushButton_61_clicked()
{
    showBtnInfo(sender());
    QCoreApplication::setOrganizationName("AXB");
    QString str=QCoreApplication::organizationName();
    ui->plainTextEdit->appendPlainText(str);
}

// QCoreApplication::exit()  退出当前应用程序
void Dialog::on_pushButton_40_clicked()
{
    QCoreApplication::exit();
}

// QFile::copy()  复制文件（静态函数版本）
void Dialog::on_pushButton_48_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    /*
    QFileInfo 是一个Qt类，用于获取文件的相关信息。通过将文件路径 sous 传递给 QFileInfo 的构造函数，可以创建一个 QFileInfo 对象。  
    fileInfo 对象可以用来获取文件的路径、基本名称、后缀名等信息。
    */
    QFileInfo fileInfo(sous);
    QString newFile=fileInfo.path()+"/"+fileInfo.baseName()+"--副本."+fileInfo.suffix();//fileInfo.path()  获取文件的路径,fileInfo.baseName()  获取文件的基本名称,fileInfo.suffix()  获取文件的后缀名。通过拼接这些信息，生成副本文件的路径
    QFile::copy(sous,newFile);//复制源文件到副本
    ui->plainTextEdit->appendPlainText("源文件:"+sous);
    ui->plainTextEdit->appendPlainText("副本："+newFile+"\n");//这两行代码将源文件路径和副本文件路径记录到文本框中，方便用户查看操作结果
}

// QFile::copy()  复制文件O（成员函数版本）
void Dialog::on_pushButton_53_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString newFile=fileInfo.path()+"/"+fileInfo.baseName()+"--副本."+fileInfo.suffix();
    QFile file(sous);//创建一个QFile对象，传入源文件路径 sous
    file.copy(newFile);//调用QFile对象的copy()函数，将源文件复制到副本文件
    ui->plainTextEdit->appendPlainText("源文件:"+sous);
    ui->plainTextEdit->appendPlainText("副本："+newFile+"\n");
}

// QFile::exists()  判断文件是否存在（静态函数版本）
void Dialog::on_pushButton_51_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    bool the=QFile::exists(sous);//判断文件是否存在，返回true或false
    ui->plainTextEdit->appendPlainText(sous);
    if(the)
        ui->plainTextEdit->appendPlainText("存在\n");
    else
        ui->plainTextEdit->appendPlainText("不存在\n");
}

// QFile::exists()  判断文件是否存在（成员函数版本）
void Dialog::on_pushButton_54_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFile file(sous);//创建一个QFile对象，传入文件路径 sous
    bool the=file.exists();//判断文件是否存在，返回true或false
    ui->plainTextEdit->appendPlainText(sous);
    if(the)
        ui->plainTextEdit->appendPlainText("存在\n");
    else
        ui->plainTextEdit->appendPlainText("不存在\n");
}

// QFile::remove()  删除文件（静态函数版本）
void Dialog::on_pushButton_49_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    if(QFile::exists(sous))
    {
        QFile::remove(sous);
        ui->plainTextEdit->appendPlainText("删除文件："+sous+"\n");
    }else
        ui->plainTextEdit->appendPlainText("文件不存在\n");
}

// QFile::remove()  删除文件（成员函数版本）
void Dialog::on_pushButton_55_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFile file(sous);
    if(file.exists())
    {
        file.remove();
        ui->plainTextEdit->appendPlainText("删除文件："+sous+"\n");
    }else
        ui->plainTextEdit->appendPlainText("文件不存在\n");
}

// QFile::rename()  重命名文件（静态函数版本）
void Dialog::on_pushButton_50_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString newFile=fileInfo.path()+"/"+fileInfo.baseName()+"--修改后的版本."+fileInfo.suffix();
    QFile::rename(sous,newFile);
    ui->plainTextEdit->appendPlainText("源文件:"+sous);
    ui->plainTextEdit->appendPlainText("修改后："+newFile+"\n");
}

// QFile::rename()  重命名文件（成员函数版本）
void Dialog::on_pushButton_56_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFile file(sous);//创建一个QFile对象，传入文件路径 sous
    QFileInfo fileInfo(sous);//创建一个QFileInfo对象，传入文件路径 sous
    QString newFile=fileInfo.path()+"/"+fileInfo.baseName()+"--修改后的版本."+fileInfo.suffix();
    file.rename(newFile);//调用QFile对象的rename()函数，将源文件重命名为新文件名
    ui->plainTextEdit->appendPlainText("源文件:"+sous);
    ui->plainTextEdit->appendPlainText("修改后："+newFile+"\n");
}

// QFile::moveToTrash()  删除文件（静态函数版本），将文件移动到回收站
void Dialog::on_pushButton_63_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    if(QFile::exists(sous))
    {
        QFile::moveToTrash(sous);
        ui->plainTextEdit->appendPlainText("删除文件："+sous+"\n");
    }else
        ui->plainTextEdit->appendPlainText("文件不存在\n");
}

// QFile::moveToTrash()  删除文件（成员函数版本），将文件移动到回收站
void Dialog::on_pushButton_64_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFile file(sous);
    if(file.exists())
    {
        file.moveToTrash();
        ui->plainTextEdit->appendPlainText("删除文件："+sous+"\n");
    }else
        ui->plainTextEdit->appendPlainText("文件不存在\n");
}

// QFileInfo::absoluteFilePath()  获取文件的绝对路径
void Dialog::on_pushButton_28_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.absoluteFilePath();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::absolutePath()  获取绝对路径
void Dialog::on_pushButton_29_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.absolutePath();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::fileName()  获取文件名
void Dialog::on_pushButton_33_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.fileName();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::filePath()  获取文件路径
void Dialog::on_pushButton_34_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.filePath();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::size()  获取文件大小
void Dialog::on_pushButton_38_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=QString::number(fileInfo.size())+" Bytes";//获取文件大小，返回值为字节数
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::path()  获取文件路径
void Dialog::on_pushButton_37_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.path();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::baseName()  获取文件基本名称
void Dialog::on_pushButton_30_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.baseName();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::completeBaseName()  获取文件完整基本名称
void Dialog::on_pushButton_31_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.completeBaseName();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::suffix()  获取文件后缀名
void Dialog::on_pushButton_39_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.suffix();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::completeSuffix()  获取文件完整后缀名
void Dialog::on_pushButton_32_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    QString str=fileInfo.completeSuffix();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QFileInfo::isDir()  判断是否是目录
void Dialog::on_pushButton_42_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editDir->text();
    QFileInfo fileInfo(sous);
    bool the=fileInfo.isDir();
    if(the)
        ui->plainTextEdit->appendPlainText("是目录");
    else
        ui->plainTextEdit->appendPlainText("不是目录");
}

// QFileInfo::isFile()  判断是否是文件
void Dialog::on_pushButton_43_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    bool the=fileInfo.isFile();
    if(the)
        ui->plainTextEdit->appendPlainText("是文件");
    else
        ui->plainTextEdit->appendPlainText("不是文件");
}

// QFileInfo::isExecutable()  判断是否是可执行文件
void Dialog::on_pushButton_35_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    bool the=fileInfo.isExecutable();
    if(the)
        ui->plainTextEdit->appendPlainText("是exe");
    else
        ui->plainTextEdit->appendPlainText("不是exe");
}

// QFileInfo::birthTime()  获取文件的创建时间
void Dialog::on_pushButton_58_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    ui->plainTextEdit->appendPlainText(fileInfo.birthTime().toString("yyyy-MM-dd hh:mm:ss"));//获取文件的创建时间
    ui->plainTextEdit->appendPlainText("");
}

// QFileInfo::lastModified()  获取文件的最后修改时间
void Dialog::on_pushButton_36_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    ui->plainTextEdit->appendPlainText(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
    ui->plainTextEdit->appendPlainText("");
}

// QFileInfo::lastRead()  获取文件的最后读取时间
void Dialog::on_pushButton_44_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);
    ui->plainTextEdit->appendPlainText(fileInfo.lastRead().toString("yyyy-MM-dd hh:mm:ss"));
    ui->plainTextEdit->appendPlainText("");
}

// QFileInfo::exists()  判断文件是否存在（静态函数版本）
void Dialog::on_pushButton_59_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    bool the=QFileInfo::exists(sous);
    if(the)
        ui->plainTextEdit->appendPlainText("是");
    else
        ui->plainTextEdit->appendPlainText("不是");
}

// QFileInfo::exists()  判断文件是否存在（成员函数版本）
void Dialog::on_pushButton_27_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editFile->text();
    QFileInfo fileInfo(sous);//创建一个QFileInfo对象，传入文件路径 sous
    bool the=fileInfo.exists();//判断文件是否存在，返回true或false
    if(the)
        ui->plainTextEdit->appendPlainText("是");
    else
        ui->plainTextEdit->appendPlainText("不是");
}

// QDir::tempPath()  获取临时目录(静态函数版本)
void Dialog::on_pushButton_10_clicked()
{
    showBtnInfo(sender());
    ui->plainTextEdit->appendPlainText(QDir::tempPath()+"\n");
}

// QDir::rootPath()  获取根目录(静态函数版本)
void Dialog::on_pushButton_9_clicked()
{
    showBtnInfo(sender());
    ui->plainTextEdit->appendPlainText(QDir::rootPath()+"\n");
}

// QDir::homePath()  获取主目录(静态函数版本)
void Dialog::on_pushButton_8_clicked()
{
    showBtnInfo(sender());
    ui->plainTextEdit->appendPlainText(QDir::homePath()+"\n");
}

// QDir::drives()  获取所有驱动器(静态函数版本)，比如C:/、D:、E:
void Dialog::on_pushButton_7_clicked()
{
    showBtnInfo(sender());
    for(auto &item:QDir::drives())
        ui->plainTextEdit->appendPlainText(item.path());

    ui->plainTextEdit->appendPlainText("");
}

// QDir::currentPath()  获取当前目录(静态函数版本)
void Dialog::on_pushButton_6_clicked()
{
    showBtnInfo(sender());
    ui->plainTextEdit->appendPlainText(QDir::currentPath()+"\n");
}

// QDir::setCurrent()  设置当前目录(静态函数版本)
void Dialog::on_pushButton_60_clicked()
{
    showBtnInfo(sender());
    QDir::setCurrent(ui->editDir->text());
    ui->plainTextEdit->appendPlainText(QDir::currentPath()+"\n");
}

// QDir::mkdir()  创建目录（成员函数版本）
void Dialog::on_pushButton_20_clicked()
{
    showBtnInfo(sender());
    QString subDir="subdir1";
    QDir    dir(ui->editDir->text());//创建一个QDir对象
    bool   ok=dir.mkdir(subDir);//调用QDir对象的mkdir()函数，创建一个子目录
    if (ok)
        ui->plainTextEdit->appendPlainText("所选目录下成功新建一个目录 "+subDir+"\n");
    else
        ui->plainTextEdit->appendPlainText("创建目录失败\n");
}

// QDir::rmdir()  删除目录（成员函数版本）
void Dialog::on_pushButton_24_clicked()
{
    showBtnInfo(sender());
    QString sous=ui->editDir->text();
    QDir dir(sous);//创建一个QDir对象，传入目录路径 sous
    bool ok=dir.rmdir(sous);//调用QDir对象的rmdir()函数，删除目录
    if (ok)
        ui->plainTextEdit->appendPlainText("成功删除所选目录\n"+sous+"\n");
    else
        ui->plainTextEdit->appendPlainText("删除失败\n");
}

// QDir::remove()  删除文件（成员函数版本）
void Dialog::on_pushButton_22_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());
    QString sous(ui->editFile->text());
    bool ok= dir.remove(sous);
    if (ok)
        ui->plainTextEdit->appendPlainText("删除文件:"+sous+"\n");
    else
        ui->plainTextEdit->appendPlainText("删除文件失败\n");
}

// QDir::rename()  重命名文件（成员函数版本）
void Dialog::on_pushButton_23_clicked()
{
    showBtnInfo(sender());

    QDir    dir(ui->editDir->text());
    QString sous=ui->editFile->text(); //源文件
    QFileInfo   fileInfo(sous);//
    QString newFile=fileInfo.path()+"/"+fileInfo.baseName()+".XYZ";
    dir.rename(sous,newFile);

    ui->plainTextEdit->appendPlainText("源文件："+sous);
    ui->plainTextEdit->appendPlainText("重命名为："+newFile+"\n");
}

// QDir::rename()  重命名目录（成员函数版本）
void Dialog::on_pushButton_26_clicked()
{
    showBtnInfo(sender());
    QString curDir=QDir::currentPath();
    QDir    lastDir(curDir);
    ui->plainTextEdit->appendPlainText("选择目录之前："+lastDir.absolutePath());

    QString aDir=QFileDialog::getExistingDirectory(this,"选择一个目录",curDir,QFileDialog::ShowDirsOnly);
    if (aDir.isEmpty())
        return;

    ui->editDir->setText(aDir);
    lastDir.setPath(aDir);
    ui->plainTextEdit->appendPlainText("选择目录之后："+lastDir.absolutePath()+"\n");
}

// QDir::removeRecursively()  删除目录及文件（成员函数版本）
void Dialog::on_pushButton_12_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());
    bool ok= dir.removeRecursively();
    if (ok)
        ui->plainTextEdit->appendPlainText("删除目录及文件成功\n");
    else
        ui->plainTextEdit->appendPlainText("删除目录及文件失败\n");
}

// QDir::absoluteFilePath()  获取绝对路径（成员函数版本）
void Dialog::on_pushButton_13_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());   //操作的目录
    QString  str=dir.absoluteFilePath(ui->editFile->text());
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QDir::absolutePath()  获取绝对路径（成员函数版本）
void Dialog::on_pushButton_14_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());   //绝对路径
    QString  str=dir.absolutePath();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QDir::canonicalPath()  获取规范路径（成员函数版本）
void Dialog::on_pushButton_15_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());   //绝对路径
    QString  str=dir.canonicalPath();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QDir::filePath()  获取文件路径（成员函数版本）
void Dialog::on_pushButton_19_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());
    QString  str=dir.filePath(ui->editFile->text());
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QDir::path()  获取路径（成员函数版本）
void Dialog::on_pushButton_65_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());
    ui->plainTextEdit->appendPlainText(dir.path()+"\n");
}

// QDir::dirName()  获取目录名称（成员函数版本）
void Dialog::on_pushButton_16_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());
    QString  str=dir.dirName();
    ui->plainTextEdit->appendPlainText(str+"\n");
}

// QDir::exists()  判断目录是否存在（成员函数版本）
void Dialog::on_pushButton_18_clicked()
{
    showBtnInfo(sender());
    QDir  dir(ui->editDir->text());
    bool  the=dir.exists();
    if (the)
        ui->plainTextEdit->appendPlainText("true \n");
    else
        ui->plainTextEdit->appendPlainText("false \n");
}

// QDir::isEmpty()  判断目录是否为空（成员函数版本）
void Dialog::on_pushButton_66_clicked()
{
    showBtnInfo(sender());
    QDir  dir(ui->editDir->text());
    bool  the=dir.isEmpty();
    if (the)
        ui->plainTextEdit->appendPlainText("true \n");
    else
        ui->plainTextEdit->appendPlainText("false \n");
}

// QDir::entryList()  获取目录下的所有文件和目录（成员函数版本）
void Dialog::on_pushButton_11_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());
    QStringList strList=dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    ui->plainTextEdit->appendPlainText("所选目录下的所有目录:");
    for(auto &item:strList)
        ui->plainTextEdit->appendPlainText(item);
    ui->plainTextEdit->appendPlainText("\n");
}

// QDir::entryList()  获取目录下的所有文件（成员函数版本）
void Dialog::on_pushButton_17_clicked()
{
    showBtnInfo(sender());
    QDir    dir(ui->editDir->text());
    QStringList strList=dir.entryList(QDir::Files);

    ui->plainTextEdit->appendPlainText("所选目录下的所有目录:");
    for(auto &item:strList)
        ui->plainTextEdit->appendPlainText(item);
    ui->plainTextEdit->appendPlainText("\n");
}

//在临时的目录下创建临时目录QTemporaryDir,同时用了QDir::tempPath()  获取临时目录和QDirTemporaryDir::setAutoRemove()  设置自动删除,用了QTemporaryDir::path()  获取临时目录的路径
void Dialog::on_pushButton_21_clicked()
{
    showBtnInfo(sender());
    ui->plainTextEdit->appendPlainText("QDir::tempPath()="+QDir::tempPath());

    QTemporaryDir dir;
    dir.setAutoRemove(true);
    ui->plainTextEdit->appendPlainText(dir.path()+"\n");
}

//在临时的目录下创建临时文件QTemporaryFile,同时用了QDir::tempPath()  获取临时目录和QDirTemporaryFile::setAutoRemove()  设置自动删除,用了QTemporaryFile::open()  打开文件
void Dialog::on_pushButton_25_clicked()
{
    showBtnInfo(sender());
    ui->plainTextEdit->appendPlainText("QDir::tempPath()="+QDir::tempPath());

    QTemporaryFile aFile;
    aFile.setAutoRemove(true);
    aFile.open();//打开文件
    ui->plainTextEdit->appendPlainText(aFile.fileName()+"\n");
    aFile.close();
}

//在指定的目录下创建临时目录QTemporaryDir,同时用了QDir::tempPath()  获取临时目录和QDirTemporaryDir::setAutoRemove()  设置自动删除,用了QTemporaryDir::path()  获取临时目录的路径
void Dialog::on_pushButton_67_clicked()
{
    showBtnInfo(sender());
    QString specDir=ui->editDir->text();//获取指定的目录
    ui->plainTextEdit->appendPlainText("指定的目录="+specDir);//绝对路径

    QTemporaryDir dir(specDir+"/TempDir_XXXXXX");//构造函数创建临时目录，传入指定的目录路径和临时目录名称，用随机的字符串替换XXXXXX
    dir.setAutoRemove(false);//希望保留
    ui->plainTextEdit->appendPlainText(dir.path()+"\n");
}

//在指定的目录下创建临时文件QTemporaryFile,同时用了QDir::tempPath()  获取临时目录和QDirTemporaryFile::setAutoRemove()  设置自动删除,用了QTemporaryFile::open()  打开文件
void Dialog::on_pushButton_69_clicked()
{
    showBtnInfo(sender());
    QString specDir=ui->editDir->text();
    ui->plainTextEdit->appendPlainText("指定的目录="+specDir);

    QTemporaryFile aFile(specDir+"/TempFile_XXXXXX.temp");
    aFile.setAutoRemove(false);
    aFile.open();
    ui->plainTextEdit->appendPlainText(aFile.fileName()+"\n");
    aFile.close();
}

//在当前目录下创建临时目录QTemporaryDir,同时用了QDir::currentPath()  获取当前目录和QDirTemporaryDir::setAutoRemove()  设置自动删除,用了QTemporaryDir::path()  获取临时目录的路径
void Dialog::on_pushButton_68_clicked()
{
    showBtnInfo(sender());
    ui->plainTextEdit->appendPlainText("当前路径="+QDir::currentPath());

    QTemporaryDir dir("SubDir_XXXXXX");//相对路径
    dir.setAutoRemove(false);
    ui->plainTextEdit->appendPlainText(dir.path()+"\n");
}

//在当前目录下创建临时文件QTemporaryFile,同时用了QDir::currentPath()  获取当前目录和QDirTemporaryFile::setAutoRemove()  设置自动删除,用了QTemporaryFile::open()  打开文件
void Dialog::on_pushButton_70_clicked()
{
    showBtnInfo(sender());
    ui->plainTextEdit->appendPlainText("当前路径="+QDir::currentPath());

    QTemporaryFile aFile("CurTempFile_XXXXXX.temp");
    aFile.setAutoRemove(false);
    aFile.open();
    ui->plainTextEdit->appendPlainText(aFile.fileName()+"\n");
    aFile.close();
}

// QFileSystemWatcher  监视文件和目录的变化
// QFileSystemWatcher::addPath()  添加监视的文件或目录
void Dialog::on_pushButton_46_clicked()
{
    showBtnInfo(sender());
    fileWatcher.addPath(ui->editDir->text());
    fileWatcher.addPath(ui->editFile->text());
    //这里用对象调用的方式，下面fileWatcher用引用

    connect(&fileWatcher,&QFileSystemWatcher::directoryChanged,
            this,&Dialog::do_directoryChanged);
    connect(&fileWatcher,&QFileSystemWatcher::fileChanged,
            this,&Dialog::do_fileChanged);
}
//do_directoryChanged()  目录发生变化的槽函数
void Dialog::do_directoryChanged(const QString &path)
{
    ui->plainTextEdit->appendPlainText(path+"目录发生了变换\n");
}
// do_fileChanged()  文件发生变化的槽函数
void Dialog::do_fileChanged(const QString &path)
{
    ui->plainTextEdit->appendPlainText(path+"文件发生了变换\n");
}

// QFileSystemWatcher::removePath()  移除监视的文件或目录
void Dialog::on_pushButton_47_clicked()
{
    showBtnInfo(sender());
    fileWatcher.removePath(ui->editDir->text());
    fileWatcher.removePath(ui->editFile->text());
   // disconnect(&fileWatcher);
}

// QFileSystemWatcher::directories()  获取监视的目录
void Dialog::on_pushButton_52_clicked()
{
    showBtnInfo(sender());
    for(auto &item:fileWatcher.directories())
        ui->plainTextEdit->appendPlainText(item);
    ui->plainTextEdit->appendPlainText("");
}

// QFileSystemWatcher::files()  获取监视的文件
void Dialog::on_pushButton_57_clicked()
{
    showBtnInfo(sender());
    for(auto &item:fileWatcher.files())
        ui->plainTextEdit->appendPlainText(item);
    ui->plainTextEdit->appendPlainText("");
}

