#ifndef USERINPUT_H
#define USERINPUT_H

#include <QWidget>
#include <QPoint>
#include <QString>
#include <QPushButton>
#include <QSpinBox>

#include <vector>
#include <queue>
#include <utility>
#include <set>

#include "mypainterwidget.h"
#include "history.h"
#include "outputdialog.h"

#define DRAW 0
#define MOVE 1
#define DELETE 2

template <class T>                              //求平方和，有时候是浮点，有时候是int
T square_sum(T a, T b) {
    return a * a + b * b;
}

inline int square_sum(QPoint p) {                      //重载了QPoint版本，因为我经常将QPoint直接当向量用
    return p.rx() * p.rx() + p.ry() * p.ry();
}

inline double square_sum(QPointF p) {
     return p.rx() * p.rx() + p.ry() * p.ry();
}

//必须提前声明history，因为history中有函数需要访问UserInput类中的成员
//且Userinput中也需要有history* 类型的成员，因此肯定先编译UserInput类，之后再编译history类
//因此需要提前声明history类，且只能使用其指针
class history;

class UserInput: public QWidget {
    friend class MainWindow;                    //声明成友元，需要使用该类的函数
    Q_OBJECT                                    //Qt格式，可忽略
public:
    UserInput(QWidget * parent);                //构造函数
    ~UserInput();                               //析构函数

    //从QWidget继承的函数
    void paintEvent(QPaintEvent * p);           //绘图
    void mousePressEvent(QMouseEvent * e);      //捕捉鼠标点击的事件
    void mouseMoveEvent(QMouseEvent * e);       //捕捉鼠标移动的事件
    void mouseReleaseEvent(QMouseEvent * e);    //捕捉鼠标松开的事件

    //成员函数
    int in_some_circle(QPoint p);               //判断这个点是否在某个原图中的圆内部
    void create_widgets();                      //创建所有的按钮以及其他控件
    void add_signal_slot();                     //给所有的控件加上事件
    void arrange_layout();                      //管理窗口布局
    bool output_data(string s);                 //接受路径作为参数，输出对偶图数据
    bool save_graph(string s);                  //接受路径作为参数，输出原图数据
    bool load_graph(string s);                  //接受路径作为参数，载入原图数据
    void check_convex(QPoint pt_gen, QPoint line_gen1, QPoint line_gen2, vector<int> & vec, vector<QPoint> & counter);
    void find_outside_cycle(vector<QPoint>& circles_here, set<int> & pos);  //找到外围环路
    void find_inside_cycles(vector<QPoint>& circles_here, set<int> & pos);  //找到内部的所有区域
    void connect_in_clockwise_order(vector<QPoint> & circles_here);         //顺时针连接所有的点
    QPoint calculate_gravity(vector<int> &polygon);                  //计算重心
    int calculate_area(vector<int> &polygon);                        //计算面积
    void apply_area();                                               //应用面积

    //成员变量（非控件）
    vector<QPoint> circles;                     //原图中的圆的圆心坐标
    vector<bool> is_using;                      //判断这个点是否在使用中 todo:是否可以不要
    vector<int> area;                           //原图中每个点的面积
    vector<QPoint> counter_circles;             //对偶图中的圆的圆心坐标
    vector<int> outside_cycle;                  //原图中的最外圈环路
    vector<int> counter_inside_circles;         //对偶图中的内部点的标号，即counter_circles中哪些点是内部点
    vector<vector<int> > lines_in_list;         //邻接链表的方式存储了对偶图中的边
    vector<vector<int> > ajacent_surface;       //保存原图中的点分别对应对偶图中的哪些点构成的面
    int **lines;                                //矩阵形式存储的原图中的边
    int **counter_lines;                        //矩阵形式存储的对偶图中的边

    QPoint cur_line_st;                         //点击时鼠标的坐标
    QPoint cur_line_ed;                         //移动时鼠标的坐标

    bool isPressed;                             //鼠标是否按下
    bool isGenerating;                          //是否正在生成对偶图
    bool isOptimazing;                          //是否生成优化对偶图

    int status = DRAW;                          //目前的绘图模式（共三种），参见第14-16行的宏定义
    int selected = -1;                          //目前选中的点

    history * his;                               //历史记录类

    //控件成员变量
    QPushButton * draw;                         //这开始的10行对应于用户界面上方的按钮和其他控件
    QPushButton * move;
    QPushButton * del;
    QPushButton * restart;
    QPushButton * simplify;
    QPushButton * undo;
    QPushButton * redo;
    QPushButton * generate;
    QPushButton * optimization;
    QPushButton * output;
    QSpinBox * spin;
    QCursor * cursor;                           //用于控制不同模式下的鼠标样式

private slots:                                  //槽：系统检测到SIGNAL后自动调用的函数
    void set_status_draw();                     //将模式设置为绘图，由点击draw按钮引发
    void set_status_move();                     //将模式设置为移动，由点击move按钮引发
    void set_status_del();                      //将模式设置为删除，由点击delete按钮引发
    void restart_widgit();                      //重新开始，由点击restart按钮引发
    void update_area();                         //
    void simplify_graph();                      //简化图形，在某些情况下调用
    void back_history();                        //历史回退，由点击undo按钮引发
    void foward_history();                      //历史前进，由点击redo按钮引发
    void generate_graph();                      //生成对偶图
    void optimization_graph();                  //生成优化对偶图
    void generate_graph_slot();                 //生成对偶图或者取消生成对偶图，由generate/stop按钮引发
    void optimization_graph_slot();             //生成优化对偶图或者取消优化队友图，由optimization/stop按钮引发
    void generate_graph_if_isGenerating();      //顾名思义，只在isGenerating为true时调用函数
    void optimization_graph_if_isOptimazing();
    void show_output_dialog();                  //显示导出对偶图的对话框，由菜单栏的output引发
    void show_save_dialog();                    //显示保存原图的对话框，由菜单栏的save引发
    void show_load_dialog();                    //显示载入圆图的对话框，由菜单栏的load引发
};

#endif // USERINPUT_H
