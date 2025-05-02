#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QString m_filename;
    template<class T> void writeByStream(T value);
    template<class T> void readByStream(T &value);//读取数据用引用传递
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnFile_clicked();

    void on_btnFont_In_clicked();

    void on_btnColor_In_clicked();

    void on_btnInt8_Write_clicked();

    void on_btnInt8_Read_clicked();

    void on_btnUInt8_Write_clicked();

    void on_btnUInt8_Read_clicked();

    void on_btnInt16_Write_clicked();

    void on_btnInt16_Read_clicked();

    void on_btnUInt16_Write_clicked();

    void on_btnUIn16_Read_clicked();

    void on_btnInt32_Write_clicked();

    void on_btnInt32_Read_clicked();

    void on_btnInt64_Write_clicked();

    void on_btnInt64_Read_clicked();

    void on_btnBool_Write_clicked();

    void on_btnBool_Read_clicked();

    void on_btnDouble_Write_clicked();

    void on_btnDouble_Read_clicked();

    void on_btnInt_Write_clicked();

    void on_btnInt_Read_clicked();

    void on_btnFloat_Write_clicked();

    void on_btnFloat_Read_clicked();

    void on_btnStr_Write_clicked();

    void on_btnStr_Read_clicked();

    void on_btnQStr_Write_clicked();

    void on_btnQStr_Read_clicked();

    void on_btnFont_Write_clicked();

    void on_btnFont_Read_clicked();

    void on_btnColor_Write_clicked();

    void on_btnColor_Read_clicked();

    void on_actClearOutput_triggered();

    void on_actSaveALL_triggered();

    void on_actReadALL_triggered();

private:
    Ui::MainWindow *ui;
};


template<class T>
void MainWindow::writeByStream(T value)
{
    QFile fileDevice(m_filename);//创建文件对象
    if(!fileDevice.open(QIODevice::WriteOnly))
        return;

    QDataStream fileStream(&fileDevice);
    fileStream.setVersion(QDataStream::Qt_6_2);
    //设置字节序在内存中存储数据的顺序
    //大端序：高位字节在前，低位字节在后；小端序：低位字节在前，高位字节在后
    if(ui->radio_BigEndian->isChecked())
        fileStream.setByteOrder(QDataStream::BigEndian);
    else
        fileStream.setByteOrder(QDataStream::LittleEndian);
    //设置浮点数精度
    if(ui->radio_Single->isChecked())
        fileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    else
        fileStream.setFloatingPointPrecision(QDataStream::DoublePrecision);

   // T data=value;
    fileStream<<value;
    fileDevice.close();//关闭文件QFile,释放资源
}
template<class T>
void MainWindow::readByStream(T &value)
{
    if(!QFile::exists(m_filename))
        return;

    QFile fileDevice(m_filename);
    if(!fileDevice.open(QIODevice::ReadOnly))
        return;

    QDataStream fileStream(&fileDevice);
    fileStream.setVersion(QDataStream::Qt_6_2);
    if(ui->radio_BigEndian->isChecked())
        fileStream.setByteOrder(QDataStream::BigEndian);
    else
        fileStream.setByteOrder(QDataStream::LittleEndian);

    if(ui->radio_Single->isChecked())
        fileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    else
        fileStream.setFloatingPointPrecision(QDataStream::DoublePrecision);

   // T data=value;
    fileStream>>value;
    fileDevice.close();
}

#endif // MAINWINDOW_H
