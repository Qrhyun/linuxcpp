对于QMainWindow中的一些无法用可视化UI添加的控件，可以使用代码进行添加

> QToolBar 是一个工具栏控件，通常用于放置按钮、下拉列表、输入框等控件，以便用户快速访问常用功能。
QStatusBar 是一个状态栏控件，通常位于窗口的底部，用于显示应用程序的状态信息，如文件名、进度条、提示信息等
QMainWindow 是 Qt 提供的一个主窗口类，用于构建具有菜单栏、工具栏、状态栏和中心部件的应用程序。

```
+-------------------+
|      QWidget      |
+-------------------+
          |
          +-------------------+
          |                   |
+---------+---------+         |
|   QMainWindow      |         |
+-------------------+         |
| + QToolBar        |         |
| + QStatusBar      |         |
| + QTextEdit       |         |
+-------------------+         |
          |       |       |         |
          |       |       |         |
+---------+       +-------+         +----------------+
| QToolBar |       | QStatusBar |       | QTextEdit  |
|----------|       |-------------|       |------------|
| + QAction|       | + QLabel    |       | - setFont  |
| + QSpinBox|       | + QProgressBar |    | - setFontPointSize |
| + QFontComboBox|   | + addPermanentWidget |   | - setFont  |
+----------+       +-------------+       +------------+
          |       |       |         |
          |       |       |         |
+---------+       +-------+         |
| QSpinBox |       | QLabel  |         |
|----------|       |----------|         |
| - setMinimum  |   | - setText  |         |
| - setMaximum  |   | - setMinimumWidth |         |
| - setValue    |   +------------------+         |
+----------+       |                   |
          |       |                   |
+---------+       |                   |
| QFontComboBox |  |                   |
|---------------|                   |
| - setMinimumWidth |                  |
| - setFont         |                  |
+-------------------+                  |
关系说明
QWidget
功能：所有 Qt 控件的基类。
作用：提供基本的控件功能，如窗口标题、边框、事件处理等。
QMainWindow
继承自：QWidget
功能：主窗口类，用于构建复杂的应用程序。
组合关系：
包含工具栏（QToolBar）
包含状态栏（QStatusBar）
包含中心部件（如 QTextEdit）
QToolBar
继承自：QWidget
功能：工具栏控件，用于放置工具按钮和其他控件。
组合关系：
包含按钮（QAction 或 QPushButton）
包含下拉列表（QComboBox）
包含输入框（QSpinBox）
QStatusBar
继承自：QWidget
功能：状态栏控件，用于显示状态信息。
组合关系：
包含标签（QLabel）
包含进度条（QProgressBar）
包含永久区域（addPermanentWidget）
QSpinBox
继承自：QAbstractSpinBox
功能：用于选择整数值的控件。
方法：
setMinimum(int)：设置最小值
setMaximum(int)：设置最大值
setValue(int)：设置当前值
QFontComboBox
继承自：QComboBox
功能：用于选择字体样式的控件。
方法：
setMinimumWidth(int)：设置最小宽度
setFont(const QFont &font)：设置当前字体
QLabel
继承自：QWidget
功能：用于显示文本或图像的控件。
方法：
setText(const QString &text)：设置显示文本
setMinimumWidth(int)：设置最小宽度
QProgressBar
继承自：QWidget
功能：用于显示进度的控件。
方法：
setMinimum(int)：设置最小值
setMaximum(int)：设置最大值
setValue(int)：设置当前值
QTextEdit
继承自：QAbstractScrollArea
功能：用于显示和编辑多行文本的控件。
方法：
setFont(const QFont &font)：设置字体
setFontPointSize(int)：设置字体大小
```
![ha](./5.jpg)
1.  操作符
使用场景：用于访问对象的成员。
适用对象：对象本身是一个实例（非指针）。
语法：对象.成员
```
class MyClass {
public:
    int value;
    void print() {
        std::cout << "Value: " << value << std::endl;
    }
};

int main() {
    MyClass obj;  // 创建一个对象实例
    obj.value = 10;  // 使用 . 操作符访问成员变量
    obj.print();     // 使用 . 操作符调用成员函数
    return 0;
}
```
2. -> 操作符
使用场景：用于访问指针所指向对象的成员。
适用对象：对象是一个指针。
语法：指针->成员
```
class MyClass {
public:
    int value;
    void print() {
        std::cout << "Value: " << value << std::endl;
    }
};

int main() {
    MyClass* ptr = new MyClass();  // 创建一个对象指针
    ptr->value = 20;  // 使用 -> 操作符访问成员变量
    ptr->print();     // 使用 -> 操作符调用成员函数
    delete ptr;       // 释放指针指向的对象
    return 0;
}
```
3. 区别总结
![1](./3.jpg)
4. 特殊情况
在现代 C++ 中，智能指针（如 std::unique_ptr 和 std::shared_ptr）也使用 -> 操作符来访问成员。这是因为智能指针重载了 -> 操作符，使其能够像普通指针一样使用。
```
#include <memory>
#include <iostream>

class MyClass {
public:
    int value;
    void print() {
        std::cout << "Value: " << value << std::endl;
    }
};

int main() {
    std::unique_ptr<MyClass> ptr = std::make_unique<MyClass>();  // 智能指针
    ptr->value = 30;  // 使用 -> 操作符访问成员变量
    ptr->print();     // 使用 -> 操作符调用成员函数
    return 0;
}
```
5. 栈分配和堆分配
在 C++ 中，是否需要使用 delete 来释放对象，取决于对象的分配方式。具体来说，这与对象是通过栈分配（自动存储期）还是通过堆分配（动态存储期）有关
```
1. 栈分配（自动存储期）
定义：对象在栈上分配，其生命周期由作用域决定。
特点：
对象在定义时自动分配内存。构造函数会在变量定义的那一刻被调用
当对象的作用域结束时，析构函数会被自动调用,内存会自动释放，不需要手动调用 delete。
通常用于局部变量。
eg:
void func(){
MyClass obj;  // 栈分配,构造函数被调用
obj.value = 10;
obj.print();
}//析构函数被调用
解释：obj 是一个栈分配的对象，当 main 函数结束时，obj 的内存会自动释放，不需要手动调用 delete。
```
```
2. 堆分配（动态存储期）
定义：对象在堆上分配，其生命周期由程序员控制。
特点：
使用 new 关键字动态分配内存。
需要手动使用 delete 释放内存，否则会导致内存泄漏。
通常用于需要动态分配内存的场景。
eg:
MyClass* ptr = new MyClass();  // 堆分配，构造函数此刻会被调用
ptr->value = 20;
ptr->print();
delete ptr;  // 必须手动释放内存，析构函数此刻会被调用。
解释：ptr 是一个指向堆分配对象的指针。使用 new 分配的内存必须用 delete 释放，否则会导致内存泄漏。
```
```
3. 智能指针
定义：智能指针（如 std::unique_ptr 和 std::shared_ptr）自动管理动态分配的内存。
特点：
智能指针在析构时自动调用 delete 释放内存。
使用智能指针可以避免手动管理内存，减少内存泄漏的风险。
eg:
std::unique_ptr<MyClass> ptr = std::make_unique<MyClass>();  // 智能指针，此刻调用构造函数。
ptr->value = 30;
ptr->print();
// 不需要手动调用 delete，智能指针会自动释放内存
解释：智能指针 ptr 在析构时会自动调用 delete 释放内存，因此不需要手动调用 delete。
```