#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);  //不知何用，自己生成的
    setMinimumSize(1280, 720);  //设置窗口尺寸
    setMaximumSize(1280, 720);

    create_action();    //创建菜单栏中的事件（即点开菜单后的选项）
    create_menu();      //创建菜单栏

    userinput = new UserInput(this);    //主要的绘图窗口
    //设置背景颜色
    userinput->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0, 188, 208));
    userinput->setPalette(palette);

    cout<<"Mainwindow construction ends"<<endl;   //调试用语句
}

//创建菜单栏中的事件
void MainWindow::create_action() {
    //保存原图的事件
    saveAction = new QAction(tr("Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save_graph()));

    //载入图的事件
    loadAction = new QAction(tr("Open"), this);
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, SIGNAL(triggered()), this, SLOT(load_graph()));

    //显示帮助文档
    readmeAction = new QAction(tr("Readme"), this);
    readmeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    connect(readmeAction, SIGNAL(triggered()), this, SLOT(show_help_contents()));

    //导出对偶图的事件
    outputAction = new QAction(tr("Output"), this);
    outputAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    connect(outputAction, SIGNAL(triggered()), this, SLOT(output_data()));
}

//创建菜单栏
void MainWindow::create_menu() {
    fileMenu = menuBar()->addMenu(tr("&File"));     //添加file选项
    fileMenu->addAction(loadAction);                //为file中添加子选项，下两行同样
    fileMenu->addAction(saveAction);
    fileMenu->addAction(outputAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));     //添加help选项
    helpMenu->addAction(readmeAction);
}

//析构函数
MainWindow::~MainWindow() {
    delete ui;
    delete userinput;
}

//保存原图的SLOT
void MainWindow::save_graph() {
    userinput->show_save_dialog();  //调用子widget中的函数
}

//导出对偶图的SLOT
void MainWindow::output_data() {
    userinput->show_output_dialog(); //调用子widget中的函数
}

//载入保存文件的SLOT
void MainWindow::load_graph() {
    userinput->show_load_dialog(); //调用子widget中的函数
}

//todo:显示帮助的SLOT
void MainWindow::show_help_contents() {

}
