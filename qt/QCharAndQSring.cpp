//这里是纯ui的交互，一些QChar和QString的例子
void Widget::on_btnGetChars_clicked()
{
    QString str=ui->editStr->text();
    if(str.isEmpty()) return;

    for(qint16 i=0;i<str.size();i++){
        QChar ch=str.at(i);
        char16_t uniCode=ch.unicode();

        QString chStr(ch);
        QString info=chStr+QString::asprintf("\t,Unicode编码=0x%X",uniCode);
        ui->plainTextEdit->appendPlainText(info);
    }
}


void Widget::on_btnClear_clicked()
{
    ui->plainTextEdit->clear();
}


void Widget::on_btnCharJudge_clicked()
{
    QString str=ui->editChar->text();
    if(str.isEmpty()) return;

    QChar ch=str.at(0);
    char16_t uniCode=ch.unicode();

    QString info=str+QString::asprintf("\t,Unicode编码=0x%X",uniCode);
    ui->plainTextEdit->appendPlainText(info);

    ui->chkDigit->setChecked(ch.isDigit()); //是否为数字0~9
    ui->chkLetter->setChecked(ch.isLetter());   //是否为字母
    ui->chkLetterOrNumber->setChecked(ch.isLetterOrNumber());   //是否为数字或字母
    ui->chkUpper->setChecked(ch.isUpper());     //是否为大写
    ui->chkLower->setChecked(ch.isLower());     //是否为小写
    ui->chkMark->setChecked(ch.isMark());       //是否为符号
    ui->chkSpace->setChecked(ch.isSpace());     //是否为空白符
    ui->chkSymbol->setChecked(ch.isSymbol());   //是否为符号
    ui->chkPunct->setChecked(ch.isPunct());     //是否为标点符号
}


void Widget::on_btnConvLatin1_clicked()
{
    QString str="Dimple";
    ui->plainTextEdit->appendPlainText(str);
    //QChar chP=QChar::fromLatin1('P');
    QChar chP='P'/*QChar('P')*/;

    str[0]=chP;
    ui->plainTextEdit->appendPlainText("\n"+str);
}


void Widget::on_btnConvUTF16_clicked()
{
    QString str="Hello,北京！";
    ui->plainTextEdit->appendPlainText(str);
    //QChar chP=QChar::fromLatin1('P');
    QString qd="青岛";
    str[6]=qd.at(0)/*QChar(0x9752)*/;
    str[7]=QChar(0x5C9B);
    ui->plainTextEdit->appendPlainText("\n"+str);
}


void Widget::on_btnCompare_clicked()
{
    QString HuStr="河to湖";
    QChar He= QChar::fromUcs2(HuStr[0].unicode());
    QChar Hu= QChar(HuStr[3].unicode());
    QString str="他们来自河南或河北";
    ui->plainTextEdit->appendPlainText(str);
    for(int i=0;i<str.size();i++)
    {
        if(str.at(i)==He)
            str[i]=Hu;
    }
    ui->plainTextEdit->appendPlainText("\n"+str);
}
