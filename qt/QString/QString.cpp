#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

//QSring类中的front()和back()函数，分别返回字符串的第一个字符和最后一个字符，同时也有ui->comboBoxStr1->currentText()函数获取当前选中的字符串
void Widget::on_btnFrontBack_clicked()
{
    ui->plainTextEdit->appendPlainText("front()，back()函数测试");
    QString str1=ui->comboBoxStr1->currentText();
    ui->plainTextEdit->appendPlainText(str1);
    ui->plainTextEdit->appendPlainText("front="+QString(str1.front()));
    ui->plainTextEdit->appendPlainText("back="+QString(str1.back()));
}

//QString类中的right()和left()函数，分别返回字符串的右侧和左侧N个字符，同时right()和left()函数的参数是从右侧或左侧开始截取的字符数，因此要传入一个整数参数
void Widget::on_btnLeftRight_clicked()
{
    ui->plainTextEdit->appendPlainText("right()，left()函数测试");
    QString str1=ui->comboBoxStr1->currentText();
    ui->plainTextEdit->appendPlainText(str1);
    ui->plainTextEdit->appendPlainText("right="+QString(str1.right(9)));
    ui->plainTextEdit->appendPlainText("left="+QString(str1.left(2)));
}

//QString类中的first()和last()函数
void Widget::on_btnFirstLast_clicked()
{
    ui->plainTextEdit->appendPlainText("first(),last()函数测试");
    QString str1=ui->comboBoxStr1->currentText();
    QString str2=ui->comboBox_2->currentText();
    ui->plainTextEdit->appendPlainText("str1:"+str1);
    ui->plainTextEdit->appendPlainText("str2:"+str2);
    //QString类中的lastIndexOf()函数，返回字符串str1中最后一个str2字符串的索引位置，也就是记录了str2字符串在str1字符串中的位置，从而达到分隔符的作用
    int N=str1.lastIndexOf(str2);
    //QString类中的first()和last()函数
    QString strFirst=str1.first(N+1);
    QString strLast=str1.last(str1.size()-N-1);

    ui->plainTextEdit->appendPlainText("路径名称："+strFirst);
    ui->plainTextEdit->appendPlainText("文件名称:"+strLast);
}

//QString类中的section()函数，即用str2字符串分隔符str1字符串,两个参数分别表示begin和end
//start：从第几个分隔符之后开始提取（从0开始计数）,end：提取到第几个分隔符之前结束，（默认为 start，即只提取一个部分）
//由此可见，section函数的返回值是从第start个分隔符到第end个分隔符之间的字符串
//由于 start 和 end 都是 N，所以这段代码只会提取第 N 个分隔符之后的部分（从0开始计数）
{
    ui->plainTextEdit->appendPlainText("section函数测试");
    QString str1=ui->comboBoxStr1->currentText();
    QString str2=ui->comboBox_2->currentText();
    int N=ui->spinBox->value();
    QString strSection=str1.section(str2,N,N);
    ui->plainTextEdit->appendPlainText(strSection);
}


void Widget::on_pushButton_clicked()
{
    ui->plainTextEdit->clear();
}

//QString类中的isNull()和isEmpty()函数，分别判断字符串是否为NULL和是否为空字符串，isNull()函数返回true表示字符串为NULL，isEmpty()函数返回true表示字符串为空字符串
void Widget::on_btnIsNullIsEmpty_clicked()
{
    ui->plainTextEdit->appendPlainText("isNull和isEmpty函数测试：");
    QString str1,str2="";//str1是NULL,str2是Empty
    ui->plainTextEdit->appendPlainText("QString str1,str2=""");

    QString str="str1.isNull()=";
    if(str1.isNull()) str=str+"true"; else str=str+"false";
    ui->plainTextEdit->appendPlainText(str);

    str="str1.isEmpty()=";
    if(str1.isEmpty()) str=str+"true"; else str=str+"false";
    ui->plainTextEdit->appendPlainText(str);

    str="str2.isNull()=";
    if(str2.isNull()) str=str+"true"; else str=str+"false";
    ui->plainTextEdit->appendPlainText(str);

    str="str2.isEmpty()=";
    if(str2.isEmpty()) str=str+"true"; else str=str+"false";
    ui->plainTextEdit->appendPlainText(str);
}

//QString的resize()函数，resize()函数的参数是一个整数，表示字符串的长度，如果字符串的长度小于参数值，则在字符串的末尾添加空格字符，直到字符串的长度等于参数值；如果字符串的长度大于参数值，则截断字符串，直到字符串的长度等于参数值
void Widget::on_btnResize_clicked()
{
    ui->plainTextEdit->appendPlainText("resize函数测试：");
    QString str;
    str.resize(5,'0');
    ui->plainTextEdit->appendPlainText(str);
    //注意这个函数是扩展，不是覆盖，因此前面5个字符是0，后面5个字符是0x54C8
    str.resize(10,QChar(0x54C8));
    ui->plainTextEdit->appendPlainText(str);
}

//QString类中的size()、count()和length()函数，分别返回字符串的长度、字符数和字节数，这里都是相同的
void Widget::on_btnSzieLength_clicked()
{
    ui->plainTextEdit->appendPlainText("size,count,lenght函数测试：");
    QString str1=ui->comboBoxStr1->currentText();
    //这里用long long类型来保存字符串的长度，避免溢出
    ui->plainTextEdit->appendPlainText(QString::asprintf("size:%lld",str1.size()));
    ui->plainTextEdit->appendPlainText(QString::asprintf("count:%lld",str1.count()));
    ui->plainTextEdit->appendPlainText(QString::asprintf("length:%lld",str1.length()));
}

//QString类中的fill()函数，fill()函数的参数是一个字符，表示用这个字符填充字符串，fill()函数的返回值是填充后的字符串
void Widget::on_btnFill_clicked()
{
    ui->plainTextEdit->appendPlainText("fill函数测试：");
    QString str="Hello";
    str.fill('X');//用字符X填充字符串，所有字符都被替换为X
    ui->plainTextEdit->appendPlainText(str);
    str.fill('A',2);//用字符A填充字符串，同时调用resize()函数，扩展字符串的长度为2,即从5个字符扩展到2个字符
    ui->plainTextEdit->appendPlainText(str);
    str.fill(QChar(0x54C8),3);//用字符0x54C8填充字符串，同时调用resize()函数，扩展字符串的长度为3
    ui->plainTextEdit->appendPlainText(str);
}

//QString类中的trimmed()和simplified()函数，分别去掉字符串两端的空格和去掉字符串中间的多余空格（即中间只保留一个空格）
void Widget::on_btnTrimmedSimplified_clicked()
{
    ui->plainTextEdit->appendPlainText("trimme,simplified函数测试");
    QString str1=ui->comboBoxStr1->currentText();

    ui->plainTextEdit->appendPlainText("str1= " +str1);

    QString str2=str1.trimmed();
    ui->plainTextEdit->appendPlainText("trimmed()= " +str2);

    str2=str1.simplified();
    ui->plainTextEdit->appendPlainText("simplified()= " +str2);
}

//QString类中的insert()函数，insert()函数的参数是一个字符串，表示在字符串的指定位置的前面插入一个字符串，insert()函数的返回值是插入后的字符串
void Widget::on_btnInsert_clicked()
{
    ui->plainTextEdit->appendPlainText("insert函数测试");
    QString str1="It is great";
    ui->plainTextEdit->appendPlainText(str1);

    int N= str1.lastIndexOf(" ");//获取最后一个空格的索引位置
    str1.insert(N, "n't");//在最后一个空格的位置的前面插入字符串"n't"
    ui->plainTextEdit->appendPlainText(str1);
}

//QString类中的remove()函数，remove()函数的参数是一个字符，表示在字符串中删除指定的字符，remove()函数的返回值是删除后的字符串
void Widget::on_btnRemove_clicked()
{
    //移除特定的字符
        ui->plainTextEdit->appendPlainText("remove移除特定的字符");
        QString str1="你们，我们，他们";
        ui->plainTextEdit->appendPlainText("str1= "+str1);

        QString DeStr="们";
        QChar   DeChar=QChar(DeStr[0].unicode());//获取字符的unicode编码
        str1.remove(DeChar);
        ui->plainTextEdit->appendPlainText("str1= "+str1);

    //移除右侧N个字符
        ui->plainTextEdit->appendPlainText("remove移除右侧N个字符");
        str1="G:\\Qt6Book\\QtSamples\\qw.cpp";//这里是写代码的形式，因此要用转义符\\，来表示一个\字符
        ui->plainTextEdit->appendPlainText(str1);

        int N=str1.lastIndexOf("\\");
        str1.remove(N+1, 20);
        ui->plainTextEdit->appendPlainText(str1);
}

//QString类中的replace()函数，replace()函数的参数是一个字符，表示在字符串中替换指定的字符，replace()函数的返回值是替换后的字符串
void Widget::on_btnReplace_clicked()
{
    //替换指定的字符
        ui->plainTextEdit->appendPlainText("replace,替换指定的字符");
        QString str1="Goooogle";
        ui->plainTextEdit->appendPlainText(str1);

        str1.replace('o','e');//将字符串中的字符o替换为字符e
        ui->plainTextEdit->appendPlainText(str1);

    //替换字符串
        ui->plainTextEdit->appendPlainText("replace,替换字符串");
        str1="It is great";
        int N= str1.lastIndexOf(" ");//获取最后一个空格的索引位置
        ui->plainTextEdit->appendPlainText(str1);

        QString subStr="wonderful";
        //从最后一个空格的下一个位置（N+1）开始替换，替换的长度为subStr的长度
        str1.replace(N+1,subStr.size(), subStr);
        ui->plainTextEdit->appendPlainText(str1);

        str1.replace(N+1, subStr.size(),"OK!");
        qDebug(str1.toLocal8Bit().data());
        ui->plainTextEdit->appendPlainText(str1);
}

