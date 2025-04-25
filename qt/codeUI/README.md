# 下划线，加粗，斜体
![1](./images/1.png)
# 颜色
![2](./images/2.png)
# 文本
![3](./images/3.png)

# 使用的库
#include <QDialog>：用于创建对话框窗口。
#include <QCheckBox>：用于创建复选框。
#include <QRadioButton>：用于创建单选按钮。
#include <QPlainTextEdit>：用于创建多行文本编辑框。
#include <QPushButton>：用于创建按钮。
#include <QHBoxLayout> 和 #include <QVBoxLayout>：用于创建水平和垂直布局管理器。

# 使用的类
QDialog：Dialog 类继承自 QDialog，用于创建模态对话框。
QCheckBox：用于创建复选框，如下划线、斜体、加粗。
QRadioButton：用于创建单选按钮，如颜色选择。
QPlainTextEdit：用于创建文本编辑框。
QPushButton：用于创建按钮，如确定、取消、退出。
QHBoxLayout 和 QVBoxLayout：用于布局管理，组织控件的排列方式。
## 类之间的关系图
```
Dialog
├── chkBoxUnder (QCheckBox)
├── chkBoxItalic (QCheckBox)
├── chkBoxBold (QCheckBox)
├── radioBlack (QRadioButton)
├── radioRed (QRadioButton)
├── radioBlue (QRadioButton)
├── txtEdit (QPlainTextEdit)
├── btnOK (QPushButton)
├── btnCancel (QPushButton)
└── btnClose (QPushButton)
Dialog 类是主类，它包含了所有的控件作为成员变量。
每个控件（如 QCheckBox、QRadioButton、QPlainTextEdit、QPushButton）都是 Dialog 类的成员，这意味着它们是 Dialog 对象的一部分。
控件通过布局管理器（QHBoxLayout 和 QVBoxLayout）组织在一起，以定义它们在对话框中的排列方式。
注意：
QCheckBox、QRadioButton、QPushButton 继承自 QAbstractButton，而 QAbstractButton 继承自 QWidget。
QPlainTextEdit 继承自 QAbstractScrollArea，而 QAbstractScrollArea 继承自 QWidget。
QHBoxLayout 和 QVBoxLayout 继承自 QLayout，而 QLayout 并不直接继承自 QWidget，但它是用于管理 QWidget 子类控件布局的。
```
# 槽函数
```
槽函数是 Dialog 类的方法，它们响应控件发出的信号：
do_chkBoxUnder(bool checked)：当用户点击下划线复选框时，这个方法会被调用，以更新文本编辑框的下划线样式。
do_chkBoxItalic(bool checked)：当用户点击斜体复选框时，这个方法会被调用，以更新文本编辑框的斜体样式。
do_chkBoxBold(bool checked)：当用户点击加粗复选框时，这个方法会被调用，以更新文本编辑框的加粗样式。
do_setFontColor()：当用户点击任何一个颜色单选按钮时，这个方法会被调用，以更新文本编辑框的字体颜色。
```
# connect
> connect 函数实际上是 QObject 类的一个成员函数。在 Qt 中，QObject 是一个核心类，提供了对象之间的通信机制，包括信号和槽系统。几乎所有的 Qt 对象都继承自 QObject，因此都可以使用 connect 函数来建立信号和槽之间的连接
