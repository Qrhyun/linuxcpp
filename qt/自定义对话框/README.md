## 自定义对话框
> ![3](2.jpg)





> 相当于又多了一个页面，所以：
![1](./0.jpg)
![2](./1.jpg)



## 不同界面之间的public接口
```
//接收主界面meanwindows的传来的参数
void TDialogSize::setRowColumn(int row, int column)
{
    ui->spinBoxRow->setValue(row);
    ui->spinBoxColumn->setValue(column);
}
//返回给主界面值
int TDialogSize::rowCount()
{
    return ui->spinBoxRow->value();
}

int TDialogSize::columnCount()
{
    return ui->spinBoxColumn->value();
}
```


## 模态和非模态
> 模态对话框（Modal Dialog）
模态对话框是指在对话框显示期间，用户必须先完成对话框中的操作（如点击“确定”或“取消”按钮），才能继续与父窗口或其他窗口交互。换句话说，模态对话框会阻塞用户对其他窗口的操作，直到对话框被关闭。在Qt中，使用 exec() 函数显示对话框时，对话框将以模态方式显示。exec() 会显示对话框，并且会阻塞代码的执行，直到对话框被关闭。对话框关闭后，**exec() 函数会返回一个值（通常是 QDialog::Accepted 或 QDialog::Rejected）**，表示用户是通过点击“确定”还是“取消”按钮关闭对话框的。在对话框显示期间，用户无法与父窗口或其他窗口交互。
`int ret = dlgTableSize->exec();`



> 非模态对话框（Non-Modal Dialog）
非模态对话框是指在对话框显示后，用户仍然可以与父窗口或其他窗口交互。非模态对话框不会阻塞代码的执行，也不会阻塞用户对其他窗口的操作。在Qt中，使用 show() 或 open() 函数显示对话框时，对话框将以非模态方式显示。show() 会显示对话框，但不会阻塞代码的执行。对话框显示后，用户可以自由地在对话框和父窗口之间切换。对话框关闭时，不会返回值，需要通过**信号和槽机制**来处理对话框的关闭事件。
`dlgTableSize->show();`

## 关于调用自定义界面的返回值的内部逻辑
![dd](./8.jpg)


## 三个例子
> 设置表格行数和列数
TDialogSize自定义类定义接口函数，然后涉及主页面model修改行列数的操作
模态的，根据自定义类的接口函数的返回值判断逻辑

> 设置表格的列头名字
TDialogHeader自定义类接口函数和QStringListModel，然后涉及主页面和自定义页面的QStringListModel修改的操作
模态的，根据自定义类的接口函数的返回值判断逻辑

> 设置单元格的内容
```
自定义对话框
void TDialogLocate::on_btnSetText_clicked()
{
    QString text=ui->edtCaption->text();
    //获取自定义对话框的行列大小
    int row=ui->spinBoxRow->value();
    int column=ui->spinBoxColumn->value();

    //在此设置自定义对话框的行列大小
    if(ui->chkBoxRow->isChecked())
        ui->spinBoxRow->setValue(ui->spinBoxRow->value()+1);

    if(ui->chkBoxColumn->isChecked())
        ui->spinBoxColumn->setValue(ui->spinBoxColumn->value()+1);
    //关于想设置主页面的单元格的内容，放在主页面中，这里自定义对话框仅仅做一个发送信号的函数
    emit changeCellText(row,column,text);
}
```
这个是非模态的，需要在自定义对话框中重写TDialogLocate类的closeEvent和showEvent函数进行非模态的实现。例如：
```
void TDialogLocate::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit changeActionEnable(true);
}

void TDialogLocate::showEvent(QShowEvent *event)
{
    event->accept();
    emit changeActionEnable(false);
}
```