#ifndef TPERSON_H
#define TPERSON_H

#include <QObject>


class TPerson : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("author","Wang")
    Q_CLASSINFO("company","UPC")
    Q_CLASSINFO("version","2.0.0")

    Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(int score MEMBER m_score)
public:
    //这里构造函数用QObject *paren作为参数，作为对象树的parent
    explicit TPerson(QString name,QObject *parent = nullptr);
    ~TPerson();
    int age();
    void setAge(quint8 ageValue);
    void incAge();

signals:
    //自定义的信号，位于signal里面，不需要实现
    void ageChanged(int ageValue);//年龄改变的时候的signal函数

private:
    //QString m_sex;
    QString m_name;
    int m_age=10;
    int m_score=79;
};

#endif // TPERSON_H
