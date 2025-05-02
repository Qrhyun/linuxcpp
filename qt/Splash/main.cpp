#include "mainwindow.h"

#include <QApplication>
#include "tlogindialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //添加启动页面
    TLoginDialog *dlgLogin=new TLoginDialog;
    //如果登录成功，返回QDialog::Accepted，跳转到主窗口mainwindow
    if(dlgLogin->exec()==QDialog::Accepted)
    {
        MainWindow w;
        w.show();
        return a.exec();
    }else
        return 0;
}
