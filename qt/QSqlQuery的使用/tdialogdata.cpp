#include "tdialogdata.h"
#include "ui_tdialogdata.h"
#include <QFileDialog>

QSqlRecord TDialogData::record()
{
    //这个函数用于获取子页面的相关功能传入的数据。都记录在m_record中。然后传给主页面，在主页面进行修改
    //设置记录数据，QSqlRecord是一个数据记录类，类似于一个数据表中的一行,这里用QSqlRecord的setValue函数设置记录的值
    m_record.setValue("empNo",ui->spinEmpNo->value());
    m_record.setValue("Name",    ui->editName->text());
    m_record.setValue("Gender",  ui->comboSex->currentText());
    m_record.setValue("Birthday",ui->editBirth->date());

    m_record.setValue("Province",    ui->comboProvince->currentText());
    m_record.setValue("Department",  ui->comboDep->currentText());

    m_record.setValue("Salary",  ui->spinSalary->value());
    m_record.setValue("Memo",    ui->editMemo->toPlainText());
    return m_record;
}

void TDialogData::setRecord(const QSqlRecord &newRecord)
{
    //这个函数是从主页面传入的数据，在子页面中进行设置
    m_record = newRecord;//通过这句话将newRecord的值赋给m_record
    ui->spinEmpNo->setEnabled(false);//工号不可修改
    setWindowTitle("Data对话框");//设置标题
    //设置各个控件的值，这里用控件的set各种函数来设置值，来源于newRecord里面的值
    ui->spinEmpNo->setValue(newRecord.value("empNo").toInt());
    ui->editName->setText(newRecord.value("Name").toString());
    ui->comboSex->setCurrentText(newRecord.value("Gender").toString());
    ui->editBirth->setDate(newRecord.value("Birthday").toDate());
    ui->comboProvince->setCurrentText(newRecord.value("Province").toString());
    ui->comboDep->setCurrentText(newRecord.value("Department").toString());
    ui->spinSalary->setValue(newRecord.value("Salary").toInt());
    ui->editMemo->setPlainText(newRecord.value("Memo").toString());
    //设置照片
    QVariant va=newRecord.value("Photo");
    if(va.isValid())
    {
        QPixmap pic;
        pic.loadFromData(va.toByteArray());
        ui->LabPhoto->setPixmap(pic.scaledToWidth(ui->LabPhoto->size().width()));
    }else
        ui->LabPhoto->clear();
}

TDialogData::TDialogData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogData)
{
    ui->setupUi(this);
}

TDialogData::~TDialogData()
{
    delete ui;
}
//导入照片
void TDialogData::on_btnSetPhoto_clicked()
{
    QString aFile=QFileDialog::getOpenFileName(this,"选择图片文件","", "照片(*.jpg)");
    if (aFile.isEmpty())
        return;

    QFile *file=new QFile(aFile);
    if(!file->open(QIODevice::ReadOnly))
        return;
    QByteArray data=file->readAll();
    file->close();

    m_record.setValue("Photo",data);//也记录在m_record中
    QPixmap pic;
    pic.loadFromData(data);
    ui->LabPhoto->setPixmap(pic.scaledToWidth(ui->LabPhoto->size().width()));
}

//清除照片
void TDialogData::on_btnClearPhoto_clicked()
{
    m_record.setNull("Photo");
    ui->LabPhoto->clear();
}

