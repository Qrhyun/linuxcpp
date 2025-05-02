#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtSql>//QtSql用作数据库操作
#include <QDataWidgetMapper>//QDataWidgetMapper用作数据库字段与widget的映射
#include "tcomboboxdelegate.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QSqlDatabase DB;//数据库对象
    QSqlTableModel *tabModel;
    QItemSelectionModel *selModel;

    TComboBoxDelegate delegateSex;
    TComboBoxDelegate delegateDepart;
    QDataWidgetMapper *dataMapper;
    void openTable();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actOpenDB_triggered();
    void do_currentChanged(const QModelIndex &current,const QModelIndex &previous);
    void do_currentRowChanged(const QModelIndex &current,const QModelIndex &previous);
    void on_actRecAppend_triggered();

    void on_actRecInsert_triggered();

    void on_actRecDelete_triggered();

    void on_actPhoto_triggered();

    void on_actPhotoClear_triggered();

    void on_actScan_triggered();

    void on_actSubmit_triggered();

    void on_actRevert_triggered();

    void on_comboFields_currentIndexChanged(int index);

    void on_radioBtnAscend_clicked();

    void on_radioBtnDescend_clicked();

    void on_radioBtnMan_clicked();

    void on_radioBtnWoman_clicked();

    void on_radioBtnBoth_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
