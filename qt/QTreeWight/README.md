#  QTreeWight和QDockWight码头组件
1. QTreeWidget是一种Item Widget组件，父类是Item Widget,不同于model-view需要从数据库中获取数据（父类是Item View），这里数据写死在Item里面
2. 将QTreeWight放进QDockWight组件里面

```
添加子目录代码的解释
void MainWindow::on_actAddFolder_triggered()
{
    //QFileDialog::getExistingDirectory()：弹出一个目录选择对话框，让用户选择一个已存在的目录。QString dir：存储用户选择的目录路径。
    QString dir=QFileDialog::getExistingDirectory();
    //检查用户是否取消了对话框或没有选择目录。
    if(dir.isEmpty()) return; 
    //ui->treeWidget->currentItem()：获取 QTreeWidget 当前选中的项目
    QTreeWidgetItem *parItem=ui->treeWidget->currentItem();
    if(parItem==nullptr) return;
    //parItem->type() != itImageItem：检查当前选中的项目是否不是特定类型（itImageItem）的项目
    if(parItem->type()!=itImageItem)
    {
        //获取目录路径的长度
        int cnt=dir.length();
        //找到目录路径中最后一个斜杠（/）的位置。
        int i=dir.lastIndexOf("/");
        提取目录路径中最后一个斜杠之后的部分，即目录名称
        QString nodeName=dir.right(cnt-i-1);

        QTreeWidgetItem *item=new QTreeWidgetItem(itGroupItem);
        QIcon icon(":/images/icons/open3.bmp");
        item->setIcon(colItem,icon);
        //设置新项目的文本为目录名称。
        item->setText(colItem,nodeName);
        //设置新项目的类型文本。
        item->setText(colItemType,"Group Item");
        //设置新项目的属性，使其可选择、可检查、可用且具有三态（可选中、未选中、部分选中）
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsAutoTristate);
        //设置新项目的选中状态为选中。
        item->setCheckState(colItem,Qt::Checked);
        //将目录路径dir存储在新项目的 UserRole 数据角色中，以便将来可以检索。
        item->setData(colItem,Qt::UserRole,QVariant(dir));
        //QTreeWidgetItem的addChild()方法可以添加子节点，从而可以建子目录
        parItem->addChild(item);
    }
}
功能概述:增加目录
弹出一个文件对话框让用户选择一个目录。
在 QTreeWidget 的当前选中项目下添加一个新的子项目，代表用户选择的目录。
```
```
void MainWindow::on_actAddFiles_triggered()
{
    //QFileDialog::getOpenFileNames()：弹出一个文件选择对话框，允许用户选择多个文件。QStringList files：存储用户选择的文件路径列表。
    QStringList files=QFileDialog::getOpenFileNames(this,"选择文件","","Images(*.jpg)");
    if(files.isEmpty()) return;
    //声明两个指向 QTreeWidgetItem 的指针，分别用于父目录和当前选中的目录。
    QTreeWidgetItem *parItem,*item;
    //获取 QTreeWidget 当前选中的项目。
    item=ui->treeWidget->currentItem();
    如果没有选中任何目录，使用 QTreeWidget 的第一个顶级目录作为默认目录。
    if(item==nullptr) item=ui->treeWidget->topLevelItem(0);
    //检查当前选中的项目是否是图像项（itImageItem），其不能作为目录，将其父目录作为目录
    if(item->type()==itImageItem)
        parItem=item->parent();
    else
        parItem=item;

    for(int i=0;i<files.size();++i)
    {
        QString fileName=files.at(i);
        QIcon icon(":/images/icons/31.ico");
        QFileInfo fileInfo(fileName);
        QString NodeText=fileInfo.fileName();
        QDateTime dateTime=fileInfo.lastModified();
        QTreeWidgetItem *item=new QTreeWidgetItem(itImageItem);
        item->setIcon(colItem,icon);
        item->setText(colItem,NodeText);
        item->setText(colItemType,"Image Item");
        item->setText(colDate,dateTime.toString("yyyy-MM-dd"));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsAutoTristate);
        item->setCheckState(colItem,Qt::Checked);
        item->setData(colItem,Qt::UserRole,QVariant(fileName));
        //添加子文件
        parItem->addChild(item);
    }
    //展开父目录，以便显示新添加的子目录。
    parItem->setExpanded(true);
}
功能概述:增加文件
弹出一个文件选择对话框让用户选择图像文件。
在 QTreeWidget 的当前选中项目或顶级项目下添加新的子项目，代表用户选择的每个图像文件。
```
```
void MainWindow::on_actDeleteItem_triggered()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if (item == nullptr) return;
    //获取父项：通过 item->parent() 获取当前项的父项。
    QTreeWidgetItem *parItem = item->parent();
    //删除当前项：调用 parItem->removeChild(item) 从父项中移除当前项，这将删除该项及其所有子项
    parItem->removeChild(item);
}
```
```
void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current==nullptr||current==previous) return;

    switch(current->type())
    {
    //当选中的是顶级项（可能是树形结构的根节点）时
    case itTopItem:
        ui->actAddFolder->setEnabled(true);
        ui->actAddFiles->setEnabled(true);
        ui->actDeleteItem->setEnabled(false);
        break;
    //当选中的是组项（可能是一个文件夹或目录）时
    case itGroupItem:
        ui->actAddFolder->setEnabled(true);
        ui->actAddFiles->setEnabled(true);
        ui->actDeleteItem->setEnabled(true);
        break;
    //当选中的是图像项时
    case itImageItem:
        ui->actAddFolder->setEnabled(false);
        ui->actAddFiles->setEnabled(true);
        ui->actDeleteItem->setEnabled(true);
        displayImage(current);
        break;
    }
}
功能：联系QTreeWidgetItem和QToolBox里面的action
是在 QTreeWidget 控件中的当前选中项发生变化时被调用的。这个函数根据当前选中项的类型来启用或禁用界面上不同的动作（例如添加文件夹、添加文件、删除项等），并且可能执行一些额外的操作，比如显示图像
```