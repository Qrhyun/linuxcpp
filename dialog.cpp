#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->checkBox_4,SIGNAL(clicked()),this,SLOT(do_FontColor()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    ui->plainTextEdit->clear();
}


void Dialog::on_checkBox_clicked(bool checked)
{
    QFont font = ui -> plainTextEdit->font();
    font.setUnderline(checked);
    ui->plainTextEdit->setFont(font);
}

void Dialog::do_FontColor()
{
    QPalette plet=ui->plainTextEdit->palette();

    if(ui->checkBox_4->isChecked())
        plet.setColor(QPalette::Text,Qt::red);

}
