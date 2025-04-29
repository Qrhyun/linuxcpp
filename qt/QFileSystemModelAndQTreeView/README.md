```
功能总结
这段代码的目的是将同一个文件系统模型（m_model）与三种不同的视图组件（树形视图、列表视图和表格视图）关联起来。
这样做的好处是，用户可以通过不同的视图方式来浏览文件系统，同时这些视图共享同一个数据模型，数据的更新会自动同步到所有视图中。
```
```
m_model->setRootPath("");
1. QFileSystemModel的setRootPath方法用于指定模型的根目录。它定义了模型开始加载文件系统数据的起始位置。调用setRootPath("")会将根目录设置为文件系统的最顶层
2. 如果不调用setRootPath，QFileSystemModel默认会加载当前工作目录（QDir::currentPath()）作为根目录。
3. 影响范围：全局影响整个QFileSystemModel对象，包括所有与该模型关联的视图（如treeView、listView、tableView）。
```
```
三个视图之间的联系
connect(ui->treeView,&QTreeView::clicked,ui->listView,&QListView::setRootIndex);
connect(ui->treeView,&QTreeView::clicked,ui->tableView,&QTableView::setRootIndex);
具体行为
当用户在treeView中点击某个节点时：
treeView发出clicked信号，并传递被点击节点的索引。
listView和tableView的setRootIndex槽函数被调用，它们的根索引被设置为treeView中被点击的节点。
listView和tableView会更新显示内容，从被点击的节点开始显示文件和文件夹
```
```
void MainWindow::on_btnApplyFilters_clicked()
{
    QStringList filter=ui->comboFilters->currentText().trimmed().split(";",Qt::SkipEmptyParts);
    m_model->setNameFilters(filter);
}
1. ui->comboFilters->currentText().trimmed()
获取当前下拉列表comboFilters中选中的文本，并使用trimmed()方法去除首尾的空白字符。
2. .split(";", Qt::SkipEmptyParts)
使用分号";"作为分隔符，将过滤条件字符串分割成多个子字符串。
参数Qt::SkipEmptyParts表示跳过空字符串，避免生成空的过滤规则。
例如，如果用户输入了"*.txt;*.cpp"，split方法会将其分割为两个字符串"*.txt"和"*.cpp"，并存储在QStringList中。
3. m_model->setNameFilters(filter)
调用QFileSystemModel的setNameFilters方法，将分割后的过滤规则列表filter设置为模型的过滤条件。
setNameFilters方法接受一个QStringList作为参数，每个字符串表示一个文件名过滤规则（如"*.txt"表示只显示扩展名为.txt的文件）。
4. 这样，QFileSystemModel会根据新的过滤规则更新文件列表，只显示符合过滤条件的文件。
```