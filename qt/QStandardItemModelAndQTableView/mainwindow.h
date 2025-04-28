#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QLabel;
class QStandardItemModel;
class QItemSelectionModel;
class QModelIndex;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    const int FixedColumnCount=6;//表格列数
    //下面的这些仅仅声明，还没有初始化
    QLabel *labCurFile;
    QLabel *labCellPos;
    QLabel *labCellText;

    //表格模型，选择模型
    QStandardItemModel *m_model;
    QItemSelectionModel *m_selection;

    void iniModelData(QStringList &aFileContnet);
private slots:
    //QItemSelectionModel作为信号函数，通过它可以获得选中单元格的模型索引
    void do_currentChanged(const QModelIndex &current,const QModelIndex &previous);

    void on_actOpen_triggered();

    void on_actModelData_triggered();

    void on_actAppend_triggered();

    void on_actInsert_triggered();

    void on_actDelete_triggered();

    void on_actAlignLeft_triggered();

    void on_actAlignCenter_triggered();

    void on_actAlignRight_triggered();

    void on_actFontBold_triggered(bool checked);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
