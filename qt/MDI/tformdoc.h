#ifndef TFORMDOC_H
#define TFORMDOC_H

#include <QWidget>

namespace Ui {
class TFormDoc;
}

class TFormDoc : public QWidget
{
    Q_OBJECT

public:
    explicit TFormDoc(QWidget *parent = nullptr);
    ~TFormDoc();

    //涉及到的操作有：1.新建文档 2.打开文档 3.保存文档 4.设置字体 5.剪切 6.复制 7.粘贴
    void loadFromFile(QString &aFileName);
    QString currentFileName();
    bool isFileOpened();
    void saveToFile();

    void setEditFont();
    void textCut();
    void textCopy();
    void textPaste();

private:
    Ui::TFormDoc *ui;
    QString m_fileName;
    bool m_fileOpened=false;
};

#endif // TFORMDOC_H
