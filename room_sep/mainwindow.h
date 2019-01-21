#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userinput.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT    //加入了Q_OBJECT，你才能使用Qt中的signal和slot机制
public:
    explicit MainWindow(QWidget *parent = 0);   //构造函数
    ~MainWindow();                              //析构函数

    void create_action();                       //创建事件，即二级菜单
    void create_menu();                         //创建菜单栏

    Ui::MainWindow *ui;                         //估计是使用默认的ui文件，可以忽略
    UserInput * userinput;                      //用户输入主界面

    QMenu * fileMenu;                           //菜单栏中的file一栏
    QMenu * helpMenu;                           //菜单栏中的help一栏
    QAction * saveAction;                       //file中的save选项
    QAction * loadAction;                       //file中的load选项
    QAction * outputAction;                     //file中的output选项
    QAction * readmeAction;                     //help中的readme选项

private slots:
    void save_graph();                          //保存原图
    void load_graph();                          //载入保存文件
    void output_data();                         //输出对偶图
    void show_help_contents();                  //todo: 现实帮助文档
};

#endif // MAINWINDOW_H
