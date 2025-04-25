#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
class QCheckBox;
class QRadioButton;
class QPlainTextEdit;
class QPushButton;

class Dialog : public QDialog
{
    Q_OBJECT
private:
    QCheckBox *chkBoxUnder;//下划线
    QCheckBox *chkBoxItalic;//斜体
    QCheckBox *chkBoxBold;//加粗

    QRadioButton *radioBlack;//颜色
    QRadioButton *radioRed;
    QRadioButton *radioBlue;

    QPlainTextEdit *txtEdit;//文本

    QPushButton *btnOK;//按钮
    QPushButton *btnCancel;
    QPushButton *btnClose;
private slots:
    void do_chkBoxUnder(bool checked);
    void do_chkBoxItalic(bool checked);
    void do_chkBoxBold(bool checked);

    //三个颜色对应同一个槽
    void do_setFontColor();

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
};
#endif // DIALOG_H
