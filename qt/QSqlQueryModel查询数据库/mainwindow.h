#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QDataWidgetMapper>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QSqlDatabase DB;
    QSqlQueryModel *qryModel;//查询模型
    QItemSelectionModel *selModel;
    QDataWidgetMapper *dataMapper;//数据映射器

    void selectData();
    void refreshTableView();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actOpenDB_triggered();
    void on_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_actRecFirst_triggered();

    void on_actRecPrevious_triggered();

    void on_actRecNext_triggered();

    void on_actRecLast_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
