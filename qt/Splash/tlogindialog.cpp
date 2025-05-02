#include "tlogindialog.h"
#include "ui_tlogindialog.h"
#include <QMouseEvent>
#include <QCryptographicHash>
#include <QSettings>
#include<QMessageBox>
TLoginDialog::TLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TLoginDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);//设置关闭时删除对象
    setWindowFlags(Qt::SplashScreen);//设置窗口样式为无边框，且不显示标题栏
   // setWindowFlags(Qt::FramelessWindowHint);设置无边框
    QApplication::setOrganizationName("AXB-QT");//设置组织名称
    QApplication::setApplicationName("sample-5");//设置应用名称
    readSettings();
}

TLoginDialog::~TLoginDialog()
{
    delete ui;
}
//加密函数
QString TLoginDialog::encrypt(const QString &str)
{
    QCryptographicHash hash(QCryptographicHash::Md5);//使用MD5加密
    hash.addData(str.toLocal8Bit());//将QString转换为QByteArray
    return hash.result();
}
//读取注册表
void TLoginDialog::readSettings()
{
    QSettings settings;//创建QSettings对象
    bool saved=settings.value("saved",false).toBool();
    m_user=settings.value("Username","user").toString();//读取用户名
    m_pswd=encrypt("12345");//调用加密函数
   // m_pswd=settings.value("PSWD",encrypt("12345")).toString();
    if(saved)
        ui->editUser->setText(m_user);

    ui->chkBoxSave->setChecked(true);
}
//写入注册表
void TLoginDialog::writeSettings()
{
    QSettings settings;
    settings.setValue("Username",m_user);
    settings.setValue("PSWD",m_pswd);
    settings.setValue("saved",ui->chkBoxSave->isChecked());
}

//重写QWight的事件处理函数
//鼠标按下事件
void TLoginDialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        m_moving=true;//进入拖动状态
        m_lastPos=event->globalPosition().toPoint()-this->pos();//获取目前鼠标位置
    }
    //最后返回父类的事件处理函数来处理事件，避免事件丢失
    return QDialog::mousePressEvent(event);
}
//重写鼠标释放事件
void TLoginDialog::mouseReleaseEvent(QMouseEvent *event)
{
    m_moving=false;//退出拖动状态
    event->accept();
}
//重写鼠标移动事件
void TLoginDialog::mouseMoveEvent(QMouseEvent *event)
{
    QPoint eventPos=event->globalPosition().toPoint();
    if(m_moving &&(event->buttons()&Qt::LeftButton)&&
            (eventPos-m_lastPos-pos()).manhattanLength()>QApplication::startDragDistance())//如果鼠标左键按下并且移动距离大于系统设置的最小拖动距离
    {
        move(eventPos-m_lastPos);
        m_lastPos=eventPos-pos();//更新鼠标位置
    }
    return QDialog::mouseMoveEvent(event);
}
//按钮点击事件
void TLoginDialog::on_btnOK_clicked()
{
    QString user=ui->editUser->text().trimmed();
    QString pswd=ui->editPSWD->text().trimmed();
    QString encrptPSWD=encrypt(pswd);
    if((user==m_user)&&(encrptPSWD==m_pswd))
    {
        writeSettings();
        this->accept();
    }else{
        m_tryCount++;
        if(m_tryCount>=M_MAXCOUNT)
        {
            QMessageBox::critical(this,"错误","输入错误次数太多！！！");
            this->reject();
        }else
            QMessageBox::warning(this,"错误提示","用户名密码错误");
    }
}

