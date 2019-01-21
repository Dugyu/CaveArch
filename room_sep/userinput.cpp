#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QPushButton>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QCursor>
#include <QAction>
#include <QPointF>
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include <vector>
#include <utility>
#include <queue>
#include <set>
#include <algorithm>
#include <fstream>
#include <math.h>

#include "userinput.h"
#include "outputdialog.h"
#include "history.h"

using namespace std;

//重置二维数组
static inline void initial_array(int ** ar) {
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            ar[i][j] = 0;
        }
    }
}


//构造函数
UserInput::UserInput(QWidget * parent) : QWidget(parent) {
    setMinimumSize(1280, 720);        //设置窗口尺寸
    setMaximumSize(1280, 720);        //设置窗口尺寸
    this->setMouseTracking(true);     //开启后，鼠标不点击也能够追踪鼠标位置，好像用不到

    create_widgets();                 //创建控件
    add_signal_slot();                //添加事件机制
    arrange_layout();                 //管理布局

    //初始化变量
    set_status_draw();                //默认模式为draw
    isPressed = false;                //鼠标没按下
    isGenerating = false;             //不生成对偶图
    isOptimazing = false;             //不开启优化
    his = new history();              //开始记录历史

    lines = new int* [100]();         //给lines矩阵分配内存空间并初始化
    counter_lines = new int* [100]();
    for (int i = 0; i < 100; i++) {
        lines[i] = new int[100]();
        counter_lines[i] = new int[100]();
    }
    cout<< "Construct function ends"<<endl; //调试语句
}


//创建所有的控件
void UserInput::create_widgets() {
    // 初始化按钮等控件
    spin = new QSpinBox();
    spin->setValue(3);
    spin->setRange(1, 10);
    cursor = new QCursor();
    setCursor(*cursor);
    move = new QPushButton("&Move");
    draw = new QPushButton("&Draw");
    del = new QPushButton("&Delete");
    restart = new QPushButton("&Restart");
    simplify = new QPushButton("&Simplify");
    undo = new QPushButton("&Undo");
    redo = new QPushButton("&Redo");
    generate = new QPushButton("&Generate");
    optimization = new QPushButton("&Optimize");
    output = new QPushButton("&Output");
}


//给控件添加事件机制
void UserInput::add_signal_slot() {
    //给按钮添加快捷键盘
    QAction *actionA = new QAction(this);
    actionA->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    undo->addAction(actionA);
    QAction *actionB = new QAction(this);
    actionB->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
    redo->addAction(actionB);

    connect(actionA, SIGNAL(triggered()), undo, SIGNAL(clicked()));
    connect(actionB, SIGNAL(triggered()), redo, SIGNAL(clicked()));

    //连接信号和槽
    connect(move, SIGNAL(clicked()),
            this, SLOT(set_status_move()));
    connect(draw, SIGNAL(clicked()),
            this, SLOT(set_status_draw()));
    connect(del, SIGNAL(clicked()),
            this, SLOT(set_status_del()));
    connect(restart, SIGNAL(clicked()),
            this, SLOT(restart_widgit()));
   // connect(spin, SIGNAL(valueChanged(int)),
            //this, SLOT(update_area()));
//    connect(spin, SIGNAL(valueChanged(int)),
//            this, SLOT(generate_graph_if_isGenerating()));
    connect(spin, SIGNAL(valueChanged(int)),
            this, SLOT(optimization_graph_if_isOptimazing()));
    connect(undo, SIGNAL(clicked()),
            this, SLOT(back_history()));
    connect(redo, SIGNAL(clicked()),
            this, SLOT(foward_history()));
    connect(generate, SIGNAL(clicked()),
            this, SLOT(generate_graph_slot()));
    connect(optimization, SIGNAL(clicked()),
            this, SLOT(optimization_graph_slot()));
    connect(output, SIGNAL(clicked()),
            this, SLOT(show_output_dialog()));
}


//管理布局
void UserInput::arrange_layout() {
    //设置窗口布局
    QVBoxLayout * layout = new QVBoxLayout();
    QHBoxLayout * top_layout = new QHBoxLayout();
    top_layout->addStretch();
    top_layout->addWidget(draw);
    top_layout->addWidget(move);
    top_layout->addWidget(del);
    top_layout->addWidget(restart);
    top_layout->addWidget(spin);
    top_layout->addWidget(undo);
    top_layout->addWidget(redo);
    top_layout->addWidget(generate);
    top_layout->addWidget(optimization);
    top_layout->addWidget(output);
    top_layout->addStretch();
    layout->addLayout(top_layout);
    layout->addStretch();
    this->setLayout(layout);
}


//绘图事件
void UserInput::paintEvent(QPaintEvent *p) {
    if (!his->can_undo())   undo->setDisabled(true);
    else                    undo->setEnabled(true);
    if (!his->can_redo())   redo->setDisabled(true);
    else                    redo->setEnabled(true);
    if (circles.empty()) {
        generate->setDisabled(true);
        optimization->setDisabled(true);
        output->setDisabled(true);
    } else {
        generate->setEnabled(true);
        optimization->setEnabled(true);
        output->setEnabled(true);
    }
    if (isGenerating) {
        optimization->setEnabled(true);
    } else {
        optimization->setDisabled(true);
    }
    QPainter painter(this);
    QPen pen;
    //绘制原图中的边
    pen.setColor(QColor(179, 235, 241));
    pen.setWidth(5);
    painter.setPen(pen);
    for (size_t i = 0; i < circles.size(); i++) {
        for (size_t j = i + 1; j < circles.size(); j++) {
            if (lines[i][j]) {
                painter.drawLine(circles[i],
                    circles[j]);
            }
        }
    }

    //绘制实时的连接线
    if (isPressed && status == DRAW) {
        pen.setWidth(5);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.drawLine(cur_line_st, cur_line_ed);
    }

    //绘制圆形以及房间面积
    int ct = 0;
    for (size_t i = 0; i < circles.size(); i++) {
        pen.setWidth(6);
        pen.setColor(QColor(18,148,182));
        painter.setPen(pen);
        painter.drawEllipse(circles[i] + QPoint(2, 2), 20, 20); //绘制阴影
        
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.setBrush(QColor(0, 188, 208));
        painter.drawEllipse(circles[i], 20, 20);            //绘制圆
        
        QString temp = QString::number(++ct);
        QString aera_t = QString::number(area[i]);
        QPoint minus;
        if (ct <= 9) minus = QPoint(5, -5);
        else minus = QPoint(8, -5);
        QFont f = painter.font();
        f.setPointSize(15);
        painter.setFont(f);
        painter.drawText(circles[i] - minus, temp);     //绘制圆中心的数字

        
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawRect(circles[i].rx() - 10,
                         circles[i].ry() - 35,
                         20, 10);
        
        f.setPointSize(10);
        painter.setFont(f);
        painter.drawText(circles[i] - QPoint(3, 25), aera_t);

    }

    //绘制选中的圆
    if (selected != -1 && !circles.empty()) {
        pen.setColor(QColor(179, 235, 241));
        pen.setWidth(6);
        painter.setPen(pen);
        QBrush b;
        painter.setBrush(b);
        painter.drawEllipse(circles[selected], 20, 20);
    }

    //绘制对偶图中的点和边
    if (isGenerating) {
        QColor color(251, 226, 81);
        pen.setColor(color);
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(QBrush(color));
        for (size_t i = 0; i < counter_circles.size(); i++) {
            painter.drawEllipse(counter_circles[i], 5, 5);
        }
        //绘制边
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(25);
        pen.setWidth(2);
        painter.setPen(pen);
        for (size_t i = 0; i < counter_circles.size(); i++) {
            for (size_t j = i + 1; j < counter_circles.size(); j++) {
                if (counter_lines[i][j]) {
                    painter.drawLine(counter_circles[i],
                        counter_circles[j]);
                }
            }
        }
        for (size_t i = 0; i < ajacent_surface.size(); i++) {
            if (isGenerating && i < ajacent_surface.size())  {
                painter.drawText(calculate_gravity(ajacent_surface[i]),
                                           QString::number(calculate_area(ajacent_surface[i])));
            }
        }
    }

}


//判断点p是否在某个圆内部
int UserInput::in_some_circle(QPoint p) {
    if (selected != -1) {                               //先判断是否是选中的那个圆
        QPoint dis = p - circles[selected];
        if (square_sum(dis) < 400)  return selected;    //20是圆的半径
    }
    for (size_t i = 0; i < circles.size(); i++) {       //遍历所有的圆，寻找点p是否在某个圆内部
        QPoint dis = p - circles[i];
        if (square_sum(dis) < 400)
            return i;
    }
    return -1;
}


//判断点mid是否在线段a-b上（容许10个像素的误差）
static bool in_line(QPoint a, QPoint b, QPoint mid) {
    if(a.rx() > b.rx()) swap(a, b);
    if (mid.rx() < a.rx() || mid.rx() > b.rx()) return false;
    double real_y = a.ry() +
            (b.ry() - a.ry()) * 1.0 * (mid.rx() - a.rx()) / (b.rx() - a.rx());
    return fabs(mid.ry() - real_y) < 10;
}


//按下鼠标的事件
void UserInput::mousePressEvent(QMouseEvent *e) {
    //删除模式下
    if (status == DELETE) {
        int k = in_some_circle(e->pos());                   //先判断点击的位置是否在某个圆内
        if (k != -1) {                                      //删除某一个圆k
            is_using[k] = false;
            for (size_t i = 0; i < circles.size(); i++) {   //将这个点连接的边置为0
                lines[i][k] = lines[k][i] = 0;
            }
            simplify_graph();                               //效率or可读性？
            his->add_his(this);                             //添加历史
            generate_graph_if_isGenerating();          //如果正在生成对偶图，则重新生成
        } else {
            bool up = false;                                //边是否有变化(update)
            for (size_t i = 0; i < circles.size(); i++) {
                for (size_t j = i + 1; j < circles.size(); j++) {
                    if (!lines[i][j]) {continue;}
                    if (in_line(circles[i], circles[j], e->pos())) {  //点击到了某一条边
                        lines[i][j] = 0;
                        up = true;
                    }
                }
            }
            if (up) {                                        //如果点到了某一条边
                his->add_his(this);                          //加入历史
                generate_graph_if_isGenerating();       //如果正在生成对偶图，则重新生成
            }
        }
        update();
    } else if (status == DRAW) {                //绘图模式下
        this->isPressed = true;                 //鼠标按下为真
        cur_line_st = e->pos();                 //保存按下的位置
        cur_line_ed = e->pos();                 //移动的位置先默认为按下位置
        int k = in_some_circle(e->pos());       //判断鼠标是否在某个圆内部
        if (k == -1) {
            circles.push_back(e->pos());        //在这个位置添加一个圆
            is_using.push_back(true);
            area.push_back(3);                  //默认面积为3
            selected = int(area.size()) - 1;    //默认这个圆为当前选中
            his->add_his(this);                 //加入历史记录
            update();                           //重新绘图
        }

    } else if (status == MOVE) {                //移动模式下
        this->isPressed = true;                 //鼠标按下为真
        cur_line_st = e->pos();                 //保存当前鼠标的位置
        int k = in_some_circle(e->pos());       //判断鼠标是否在某个圆内部
        if (k != -1) {
            selected = k;                       //选中该圆
            spin->setValue(area[k]);            //将数字框中现实这个圆的面积
        }
        update();
    }
}


//鼠标移动事件
void UserInput::mouseMoveEvent(QMouseEvent *e) {
    if (this->isPressed) {                        //鼠标按下时移动才有效
        if (status == DRAW)  {
            cur_line_ed = e->pos();               //实时的连接线
            update();
        } else if (status == MOVE) {              //挪动圆
            int k = in_some_circle(cur_line_st);
            if (k != -1) {
                circles[k] = e->pos();
                cur_line_st = e->pos();
                generate_graph_if_isGenerating();
            }
            update();
        }
    }
}


//鼠标释放事件
void UserInput::mouseReleaseEvent(QMouseEvent *e) {
    this->isPressed = false;
    if (status == DRAW) {
        int a = in_some_circle(cur_line_st);
        int b = in_some_circle(cur_line_ed);
        if (a != -1 && b != -1 && a != b) {         //线段的两端点在两不同的圆中即连线
            lines[a][b] = lines[b][a] = 1;
            cur_line_ed = cur_line_ed = QPoint(0,0);
            his->add_his(this);
            generate_graph_if_isGenerating();
        }
        update();
    } else if (status == MOVE) {                    //这有一个问题就是不移动也会加入历史
        his->add_his(this);                         //移动之后放开鼠标也就加入历史
        generate_graph_if_isGenerating();
        update();
    }
}


//将模式设置为draw,即关闭draw的点击，并打开另外两个按钮的点击
void  UserInput::set_status_draw() {
    status = DRAW;
    cursor->setShape(Qt::ArrowCursor);
    setCursor(*cursor);
    draw->setDisabled(true);
    move->setEnabled(true);
    del->setEnabled(true);
    update();
}


//同上
void  UserInput::set_status_move() {
    status = MOVE;
    cursor->setShape(Qt::OpenHandCursor);
    setCursor(*cursor);
    draw->setEnabled(true);
    move->setDisabled(true);
    del->setEnabled(true);
    update();
}


//同上
void  UserInput::set_status_del() {
    status = DELETE;
    cursor->setShape(Qt::CrossCursor);
    setCursor(*cursor);
    draw->setEnabled(true);
    move->setEnabled(true);
    del->setDisabled(true);
    update();
}


//重新开始绘图
void UserInput::restart_widgit() {
    //初始化所有成员变量，和 构造函数中相同
    status = DRAW;
    isPressed = false;
    isGenerating = false;
    generate->setText("&Generate");
    initial_array(lines);
    initial_array(counter_lines);
    circles.clear();
    is_using.clear();
    area.clear();
    counter_circles.clear();
    his->add_his(this);
    update();
}


//spin中更改后实时反应到aera数组中
void UserInput::update_area() {
    if (selected >= 0) {
        area[selected] = spin->value();
        update();
    }
}


//简化点和边，即删除某个点后将所有点重新编号，并重新生成边的矩阵
void UserInput::simplify_graph() {
    vector<QPoint> new_circle;
    vector<int> new_aera;
    vector<bool> new_using;
    vector<int> mp(circles.size());
    for (size_t i = 0; i < circles.size(); i++) {
        if (is_using[i]) {
            new_circle.push_back(circles[i]);
            new_using.push_back(true);
            new_aera.push_back(area[i]);
            mp[i] = new_circle.size() - 1;
        }
    }
    if (new_circle.empty()) {           //即界面中没有点
        circles.clear();
        is_using.clear();
        area.clear();
        return;
    }
    int ** temp = new int* [100]();
    for (int i = 0; i < 100; i++) {
        temp[i] = new int[100];
    }
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            temp[i][j] = lines[i][j];
            lines[i][j] = 0;
        }
    }
    for (size_t i = 0; i < is_using.size(); i++) {
        for (size_t j = 0; j < is_using.size(); j++) {
            if (temp[i][j] > 0 && is_using[i] && is_using[j]) {
                lines[mp[i]][mp[j]] = 1;
            }
        }
    }
    for (int i = 0; i < 100; i++) {
        delete[] temp[i];
    }
    delete[] temp;
    swap(circles, new_circle);
    swap(area, new_aera);
    swap(is_using, new_using);
    update();
}


QPoint vector_perpend(QPointF p, double len = 100.0) {                  //获取一个垂直的单位向量（100）
    QPointF q(p.ry() * -1, p.rx());
    double dis = sqrt(square_sum(q.rx(), q.ry())) / len;
    return QPoint(int(q.rx() / dis), int(q.ry() / dis));
}


QPoint unit_vector(QPointF q) {                     //获得一个同方向的单位向量（120）
    double dis = sqrt(square_sum(q.rx(), q.ry())) / 120;
    return QPoint(int(q.rx() / dis), int(q.ry() / dis));
}


//计算两个向量之间的逆时针夹角
double get_counter_rotate_angle(QPointF first, QPointF second) {
    double dist, dot, ep = 1.0e-6, angle;
    dist = sqrt(square_sum(first));
    first /= dist;
    dist = sqrt(square_sum(second));
    second /= dist;
    dot = first.rx() * second.rx() + first.ry() * second.ry();      //计算点积
    if (fabs(dot - 1.0) < ep)           return 0.0;                 //夹角为0
    else if (fabs(dot + 1.0) < ep)      return 180.0;               //夹角180
    else {
        double cross;
        angle = acos(dot);
        cross = first.rx() * second.ry() - first.ry() * second.rx();  //计算叉乘
        if (cross > 0) {                                              //说明算出来的夹角是顺时针的
            angle = 2 * acos(-1.0) - angle;
        }
        return angle * 180 / acos(-1.0);
    }
}


//计算两个向量之间的逆时针夹角，委托给上面的函数即可
double get_rotate_angle(QPointF fir, QPointF sec) {
    return 360 - get_counter_rotate_angle(fir, sec);
}


//对于某个顶点cur而言，给定边(cur, pre)，找到另一条边(cur, ret)，使得从(cur, pre)到(cur, ret)逆时针转过的角度最大
int counterclockwise_most(vector<QPoint> & circles ,vector<vector<int> > & lines_in_list,
                          int pre, int cur) {
    double maximum = -1;
    int ret = 0;
    for (size_t i = 0; i < lines_in_list[cur].size(); i++) {
        int next = lines_in_list[cur][i];
        if (next == pre) continue;
        QPoint dcurr = circles[pre] - circles[cur];
        QPoint dnext = circles[next] - circles[cur];
        double angle = get_counter_rotate_angle(dcurr, dnext);
        if (angle > maximum) {
            maximum = angle;
            ret = next;
        }
    }
    return ret;
}


//对于某个顶点cur而言，给定边(cur, pre)，找到另一条边(cur, ret)，使得从(cur, pre)到(cur, ret)逆时针转过的角度最小
int counterclockwise_least(vector<QPoint> & circles ,vector<vector<int> > & lines_in_list,
                          int pre, int cur) {
    double minimum = 361;
    int ret = 0;
    for (size_t i = 0; i < lines_in_list[cur].size(); i++) {
        int next = lines_in_list[cur][i];
        if (next == pre) continue;
        QPoint dcurr = circles[pre] - circles[cur];
        QPoint dnext = circles[next] - circles[cur];
        double angle = get_counter_rotate_angle(dcurr, dnext);
        if (angle < minimum) {
            minimum = angle;
            ret = next;
        }
    }
    return ret;
}


//对于某个顶点cur而言，给定向量(0, -1)，找到一条边(cur, ret)，使得从(0, -1)到(cur, ret)顺时针转过的角度最大
int clockwise_most(vector<QPoint>& circles ,vector<vector<int> > & lines_in_list, int cur) {
    double mimimum = 361;
    int ret = 0;
    for (size_t i = 0; i < lines_in_list[cur].size(); i++) {
        int next = lines_in_list[cur][i];
        QPoint dcurr(0, -1);
        QPoint dnext = circles[next] - circles[cur];
        double angle = get_counter_rotate_angle(dcurr, dnext);
        if (angle < mimimum) {
            mimimum = angle;
            ret = next;
        }
    }
    return ret;
}


//点击generate按钮的事件
void UserInput::generate_graph_slot() {
    if (generate->text() == QString("&Generate")) {
        isGenerating = true;
        generate->setText(QString("&Stop"));
        generate_graph();
        update();
    } else {
        generate->setText(QString("&Generate"));
        isGenerating = false;
        optimization->setText(QString("&Optimize"));
        isOptimazing = false;
        //optimization_graph();
        counter_circles.clear();
        outside_cycle.clear();
        counter_inside_circles.clear();
        ajacent_surface.clear();
        lines_in_list.clear();
        int i;
        for (i=0; i<100; i++) {
            area[i] = 1;
            update();
        }
        update();
    }
}


//点击optimization按钮的事件
void UserInput::optimization_graph_slot() {
    //if (optimization->text() == QString("&Optimize")) {
        isOptimazing = true;
       // optimization->setText(QString("&Stop"));
        optimization_graph();
        generate_graph();
        update();
    /*} else {
        optimization->setText(QString("&Optimize"));
        isOptimazing = false;

        generate_graph();
        update();
    }*/
}


//找到最外圈的环路
void UserInput::find_outside_cycle(vector<QPoint> & circles_here, set<int> & pos) {
    //找到最外侧的环路
    size_t st = *max_element(pos.begin(), pos.end(), [&circles_here](int a, int b) {    //找到最左侧的点
        return circles_here[a].rx() > circles_here[b].rx();
    });
    outside_cycle.push_back(st);
    int sec = clockwise_most(circles_here, lines_in_list, st);
    size_t pre = st, cur = sec, next, ct = 0;
    //todo:这个经常死机，带排查
    do {
        next = counterclockwise_least(circles_here, lines_in_list, pre, cur);
        QPoint f1 = circles_here[next] - circles_here[cur], f2 = circles_here[pre] - circles_here[cur];
        QPoint ad = f1 + f2;
        counter_circles.push_back(circles_here[cur] - unit_vector(ad));
        ajacent_surface[cur].push_back(int(counter_circles.size()) - 1);
        outside_cycle.push_back(cur);
        pre = cur;
        cur = next;
//        cout<<"In outside circle"<<endl;
    } while (++ct <= circles_here.size() && next != st);

    if (ct <= circles_here.size()) {    //说明有环路
        QPoint f1 = circles_here[outside_cycle[1]] - circles_here[outside_cycle[0]],
               f2 = circles_here[outside_cycle.back()] - circles_here[outside_cycle[0]],
               ad = f1 + f2;
        counter_circles.push_back(circles_here[outside_cycle[0]] - unit_vector(ad));        //将最外侧环路上的点复制一份
        ajacent_surface[outside_cycle[0]].push_back(int(counter_circles.size()) - 1);       //关联到相关的顶点，以便后续连线

        //找到最外环路中的所有线段，从其中点向外作垂线
        outside_cycle.push_back(outside_cycle[0]);
        for (int i = 0; i < int(outside_cycle.size()) - 1; i++) {
            QPoint vec = circles_here[outside_cycle[i + 1]] - circles_here[outside_cycle[i]];
            QPoint mid = (circles_here[outside_cycle[i + 1]] + circles_here[outside_cycle[i]]) / 2;
            QPoint ad = vector_perpend(vec);
            ad += mid;
            counter_circles.push_back(ad);
            ajacent_surface[outside_cycle[i]].push_back(int(counter_circles.size()) - 1);
            ajacent_surface[outside_cycle[i + 1]].push_back(int(counter_circles.size()) - 1);
        }
    } else {     //没有环路直接弃疗，删图跑路
        ajacent_surface.clear();
        counter_circles.clear();
    }
}


//找到内部的所有面
void UserInput::find_inside_cycles(vector<QPoint> & circles_here, set<int> & pos) {
    //找到图中的所有面域
    while (true) {
//        cout<<"In inside circle"<<endl;
        if (pos.empty()) break;                         //所有点都删除，则循环结束
        bool con = false;
        for (size_t i = 0; i < lines_in_list.size(); i++) {
            if (lines_in_list[i].empty()) {             //删除没有连接边的点
                auto k = pos.find(i);
                if (k != pos.end()) {
                    con = true;
                    pos.erase(k);
                }
            } else if (lines_in_list[i].size() == 1) {  //删除度为1的点
                con = true;
                int counter = lines_in_list[i][0];
                auto k = std::find(lines_in_list[counter].begin(), lines_in_list[counter].end(), i);
                lines_in_list[counter].erase(k);
                lines_in_list[i].clear();
            }
        }
        if (con) continue;            //一旦发生了删除操作，则需要重新开始
        if (pos.empty()) break;       //所有点都已经被删除，则循环结束
        vector<int> cycle;            //临时保存当前这个环路
        int st = *max_element(pos.begin(), pos.end(), [&circles_here](int a, int b) {    //找到最左侧的点
            return circles_here[a].rx() > circles_here[b].rx();
        });
        cycle.push_back(st);
        int sec = clockwise_most(circles_here, lines_in_list, st);
        int pre = st, cur = sec, next;
        do {
            cycle.push_back(cur);
            next = counterclockwise_most(circles_here, lines_in_list, pre, cur);
            pre = cur;
            cur = next;
        } while (next != st);
        //删除第一条边，因为显然这条边不在两个环中，因为用了邻接表存储，所以需要删除两次
        auto k = std::find(lines_in_list[st].begin(), lines_in_list[st].end(), sec);
        lines_in_list[st].erase(k);
        k = std::find(lines_in_list[sec].begin(), lines_in_list[sec].end(), st);
        lines_in_list[sec].erase(k);
        //求这个环路上点的重心
        QPoint p(0,0);
        int total_weight = 0;
        for (size_t i = 0; i < cycle.size(); i++) {
            p += circles_here[cycle[i]];
            total_weight ++;
        }
        p /= cycle.size();      //获得了一个面的重心
        /*****************面积控制****************/
        QPoint gravity = p;     //暂存起来，因为p还需要根据面积调整
        for (size_t i = 0; i < cycle.size(); i++) {
            QPoint v = gravity - circles_here[cycle[i]];
            p += ((area[cycle[i]]) * 0.5 / total_weight) * v;
        }
        /*****************面积控制****************/
        counter_circles.push_back(p);
        counter_inside_circles.push_back(int(counter_circles.size()) - 1);   //和这个点相连的边显然都是需要开门的
        for (size_t i = 0; i < cycle.size(); i++) {
            ajacent_surface[cycle[i]].push_back(int(counter_circles.size()) - 1);   //将这个面和顶点关联起来，以便后续连接边
        }

    }
}


//判断pt_gen是否在line_gen1和line_gen2之间
void UserInput::check_convex(QPoint pt_gen, QPoint line_gen1, QPoint line_gen2, vector<int> & vec, vector<QPoint> & counter) {
    size_t i;
    for (i = 0; i < vec.size(); i++) {
        if (counter[vec[i]] == pt_gen) {
            break;
        }
    }
    if (counter[vec[(i + vec.size() - 1) % vec.size()]] == line_gen1 && counter[vec[(i + 1) % vec.size()]] == line_gen2
      ||counter[vec[(i + vec.size() - 1) % vec.size()]] == line_gen2 && counter[vec[(i + 1) % vec.size()]] == line_gen1)
    {
        return;
    }
    vec.erase(vec.begin() + i);
}


//按照顺时针/逆时针连接顶点
void UserInput::connect_in_clockwise_order(vector<QPoint> & circles_here) {
    //将每个点的外围点都排序并首位相连
    vector<QPoint> &counter_temp = counter_circles;
    for (size_t i = 0; i < ajacent_surface.size(); i++) {
        if (ajacent_surface[i].size() < 3) continue;
        QPoint center = circles_here[i], any_vector(55, 55);
        QPoint pt_gen = counter_temp[ajacent_surface[i][0]],            //newly added
            line_gen1 = counter_temp[ajacent_surface[i][1]],
            line_gen2 = counter_temp[ajacent_surface[i][2]];
        //先按照对任意向量的顺逆时针排序
        sort(ajacent_surface[i].begin(), ajacent_surface[i].end(), [&center, &counter_temp, any_vector](int a, int b) {
            return get_counter_rotate_angle(any_vector, counter_temp[a] - center)
                    < get_counter_rotate_angle(any_vector, counter_temp[b] - center);
        });

        //只有最外围的点需要check
        if (std::find(outside_cycle.begin(), outside_cycle.end(), i) != outside_cycle.end()) {
            cout << i << " is in outside" << endl;
            check_convex(pt_gen, line_gen1, line_gen2, ajacent_surface[i], counter_temp);
        }

        ajacent_surface[i].push_back(ajacent_surface[i][0]);            //末尾push一个第一项，保证能够首位相连
        for (size_t j = 0; j < ajacent_surface[i].size() - 1; j++) {
            int x = ajacent_surface[i][j], y = ajacent_surface[i][j + 1];
            if (x != y) {
                counter_lines[x][y] = counter_lines[y][x] = 1;
            }
        }
    }
}


//计算面积
int UserInput::calculate_area(vector<int> &polygon) {
    if (polygon.empty() || polygon.size() <= 3) {
        return 0;
    }
    int area = 0.;
    for (size_t i = 0; i < polygon.size() - 1; i++) {
        area += -counter_circles[polygon[i]].rx() * counter_circles[polygon[i + 1]].ry() +
                counter_circles[polygon[i]].ry() * counter_circles[polygon[i + 1]].rx();
    }
    return abs(area / 400);
}


//计算重心
QPoint UserInput::calculate_gravity(vector<int> &polygon) {
    if (polygon.empty() || polygon.size() <= 3) {
        return QPoint(0, 0);
    }
    size_t x = 0, y = 0, n = polygon.size() - 1;
    for (size_t i = 0; i < n; i++) {
        x += counter_circles[polygon[i]].rx();
        y += counter_circles[polygon[i]].ry();
    }
    return QPoint(x / n, y / n);
}


//控制面积
void UserInput::apply_area() {
    //获取内部点的下标
    vector<bool> is_outside_circle(circles.size(), false);
    vector<int> inside_circles;
    for (size_t i = 0; i < outside_cycle.size(); i++) {
        is_outside_circle[outside_cycle[i]] = true;
    }
    for (size_t i = 0; i < is_outside_circle.size(); i++) {
        if (!is_outside_circle[i]) {
            inside_circles.push_back(i);
        }
    }
    //记录每个对偶点的度数
    vector<int> degree(counter_circles.size(), 0);
    for (size_t i = 0; i < counter_circles.size(); i++) {
        for (size_t j = i + 1; j < counter_circles.size(); j++) {
            if (counter_lines[i][j]) {
                degree[i]++;
                degree[j]++;
            }
        }
    }
    //记录当前每个区域的面积
    vector<int> current_area, expect_area;
    for (size_t i = 0; i < ajacent_surface.size(); i++) {
        current_area.push_back(calculate_area(ajacent_surface[i]));
        cout << calculate_area(ajacent_surface[i]) << endl;
    }
    //以某个内部点为基准，确定每个区域应该的面积
    int base = 0, max = 0;
    for (size_t i = 0; i < outside_cycle.size() - 1; i++) {
        if (area[outside_cycle[i]] > max) {
            max = area[outside_cycle[i]];
            base = outside_cycle[i];
        }
    }
//    if (!inside_circles.empty()) {
//        base = inside_circles[0];
//    }
    cout << base << ' ' << current_area[base] << endl;
    for (size_t i = 0; i < current_area.size(); i++) {
        expect_area.push_back(current_area[base] * area[i] / area[base]);
        cout << expect_area[i] << endl;
    }
    //确定内部点的度

    //确定内部房间
    for (size_t i = 1; i < inside_circles.size(); i++) {
        size_t j, c = inside_circles[i];
        for (j = 0; j < ajacent_surface[c].size(); j++) {
//            if ()
        }
    }

    //确定外部房间
    for (size_t i = 0; i < outside_cycle.size() - 1; i++) {
        if (base == outside_cycle[i]) continue;
        size_t j, c = outside_cycle[i];
        for (j = 0; j < ajacent_surface[c].size() - 1; j++) {
            if (degree[ajacent_surface[c][j]] == 2) {       //找到可以移动的点
                break;
            }
        }
        if (j < ajacent_surface[c].size() - 1) {
            int k = ajacent_surface[c][j];                  //这个点可以移动
            //找它所在面上的相邻点
            int neighbor_1 = ajacent_surface[c][(j - 1 + ajacent_surface[c].size() - 1) % (ajacent_surface[c].size() - 1)],
                neighbor_2 = ajacent_surface[c][(j + 1) % (ajacent_surface[c].size() - 1)];
            cout << k << ' ' << neighbor_1 << ' ' << neighbor_2 << endl;
            vector<int> temp{k, neighbor_1, neighbor_2, k};
            int triangle_origin = calculate_area(temp),
                triangle_after  = triangle_origin + expect_area[c] - current_area[c];
            QPoint mid = (counter_circles[neighbor_1] + counter_circles[neighbor_2]) / 2;
            QPoint dif = counter_circles[neighbor_2] - counter_circles[neighbor_1];
            QPoint vec = counter_circles[k] - mid;
//            QPoint vec =  vector_perpend(dif, 2.0 * (expect_area[c] - current_area[c]));
            vec = vec * triangle_after / triangle_origin;
            counter_circles[k] = mid + vec;
        }
    }
    cout << "x " << outside_cycle.size() << ' ' << inside_circles.size() << endl;
}


//生成对偶图
void UserInput::generate_graph() {
    isGenerating = true;                                  //先重置之前的所有数据
    counter_circles.clear();
    outside_cycle.clear();
    counter_inside_circles.clear();
    ajacent_surface.clear();
    lines_in_list.clear();

    lines_in_list = vector<vector<int> >(circles.size(), vector<int>());    //邻接表存储
    ajacent_surface = vector<vector<int> >(circles.size(), vector<int>());  //确定每个点周围的面的中心点
    set<int> pos;                                                           //保存所有的点的下标（因为需要重新排序）
    for (size_t i = 0; i < circles.size(); i++) {
        pos.insert(i);
    }

    for (size_t i = 0; i < circles.size(); i++) {                           //构造邻接表
        for (size_t j = i + 1; j < circles.size(); j++) {
            if (lines[i][j]) {
                lines_in_list[i].push_back(j);
                lines_in_list[j].push_back(i);
            }
        }
    }

    find_outside_cycle(circles, pos);       //找到外圈的环路
    find_inside_cycles(circles, pos);       //找到内部的所有面
    initial_array(counter_lines);           //初始化
    connect_in_clockwise_order(circles);    //顺时针连接所有点
//    apply_area();                           //控制面积
}

//生成对偶图
void UserInput::optimization_graph() {
    isOptimazing = true;                                  //先重置之前的所有数据
    bool temp = isGenerating;
    generate_graph();
    isGenerating = temp;
    if (selected >= 0) {
        area[selected] = spin->value();
        update();
    }
    update();
}


//如果点击了按钮，就生成对偶图
void UserInput::generate_graph_if_isGenerating() {
    if (isGenerating) generate_graph();
}


//如果点击了按钮，就生成优化对偶图
void UserInput::optimization_graph_if_isOptimazing() {
    if (isOptimazing) optimization_graph();
}


//导出对偶图（按需求导出）
bool UserInput::output_data(string s) {
    generate_graph();     //导出之前确保已经生成了对偶图
    ofstream out;
    try {
        out.open(s);
        vector<bool> is_inside_circle(counter_circles.size(), false);
        vector<bool> has_used(counter_circles.size(), false);
        map<int, int> mp;
        for (size_t i = 0; i < counter_inside_circles.size(); i++) {
            is_inside_circle[counter_inside_circles[i]] = true;
        }
        for (size_t i = 0; i < counter_circles.size(); i++) {
            for (size_t j = i; j < counter_circles.size(); j++) {
                if (counter_lines[i][j]) {
                    has_used[i] = has_used[j] = true;
                }
            }
        }
        int j = 0;
        for (size_t i = 0; i < has_used.size(); i++) {
            if (has_used[i]) {
                mp[i] = j++;
            }
        }
        out<< j  <<endl;
        for (size_t i = 0; i < counter_circles.size(); i++) {
            if (has_used[i]) {
                out << counter_circles[i].rx() << ' ' << counter_circles[i].ry() << " 0"<<endl;
            }
        }
        for (size_t i = 0; i < counter_circles.size(); i++) {
            for (size_t j = i; j < counter_circles.size(); j++) {
                if (counter_lines[i][j]) {
                    if (is_inside_circle[i] || is_inside_circle[j]) out << mp[i] << ' ' << mp[j] << " 0" << endl;
                    else out << mp[i] << ' ' << mp[j] << " 1" << endl;
                }
            }
        }
        out.close();
        return true;    //导出成功地标记
    } catch(exception e) {
        return false;   //导出失败的标记
    }
}


//导出能够复原图的所有函数
bool UserInput::save_graph(string s) {
    ofstream out;
    try {
        out.open(s);
        out<<"Password"<<endl;  //确保不会读取了不合法的文件而造成死机
        out<<circles.size()<<endl;
        for (size_t i = 0; i < circles.size(); i++) {
            out << circles[i].rx() << ' ' << circles[i].ry() << endl;
            out << area[i] << endl;
        }
        for (size_t i = 0; i < circles.size(); i++) {
            for (size_t j = 0; j < circles.size(); j++) {
                out << lines[i][j] <<' ';
            }
            out << endl;
        }
        out << selected << endl;
        out.close();
        return true;
    } catch(exception e) {
        return false;
    }
}


//载入原图
bool UserInput::load_graph(string s) {
    ifstream in;
    try {
        in.open(s);
        string s;
        in>>s;
        if (s != "Password") return false;
//        circles.clear();
//        aera.clear();
//        initial_array(lines);
        restart_widgit();
        int n;
        in>>n;
        for (int i = 0; i < n; i++) {
            int x, y, z;
            in >> x >> y >> z;
            circles.push_back(QPoint(x, y));
            area.push_back(z);
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                in >> lines[i][j];
            }
        }
        in >> selected;
        in.close();
        return true;
    } catch (exception e) {
        return false;
    }

}


//显示对话框
void UserInput::show_load_dialog() {
    QFileDialog * fd = new QFileDialog(this);
    fd->setFileMode(QFileDialog::ExistingFile);
    fd->setFocus(Qt::MouseFocusReason);
    fd->setFocusPolicy(Qt::NoFocus);
    QString file;
    if (fd->exec() == QDialog::Accepted) {
        file = fd->selectedFiles()[0];
    }
    if (!file.isEmpty()) {
        if (load_graph(file.toStdString())) {
            update();
            isGenerating = false;
            QMessageBox::information(NULL, tr("Path"), tr("Load success!"));
        } else {
            QMessageBox::information(NULL, tr("Path"), tr("Load Failure......"));
        }
    }
    delete fd;
    his->add_his(this);
}


//显示对话框
void UserInput::show_output_dialog() {
    QFileDialog * fd = new QFileDialog(this);
    fd->setFileMode(QFileDialog::Directory);
    fd->setFocus(Qt::MouseFocusReason);
    fd->setFocusPolicy(Qt::NoFocus);
    QString dir;
    QString file("/output.txt");                    //window移植问题！
    if(fd->exec() == QDialog::Accepted) {
        dir=fd->selectedFiles()[0] + file;
    }
    cout<<dir.toStdString()<<endl;
    if (!dir.isEmpty()) {
        if (output_data(dir.toStdString())) {
            QMessageBox::information(NULL, tr("Path"), tr("Output success!"));
        } else {
            QMessageBox::information(NULL, tr("Path"), tr("Output Failure......"));
        }
    }
    delete fd;
}


//显示对话框
void UserInput::show_save_dialog() {
    QFileDialog * fd = new QFileDialog(this);
    fd->setFileMode(QFileDialog::Directory);
    fd->setFocus(Qt::MouseFocusReason);
    fd->setFocusPolicy(Qt::NoFocus);
    QString dir;
    QString file("/graph.sp");                      //window移植问题！
    if(fd->exec() == QDialog::Accepted) {
        dir=fd->selectedFiles()[0] + file;
    }
    cout<<dir.toStdString()<<endl;
    if (!dir.isEmpty()) {
        if (save_graph(dir.toStdString())) {
            QMessageBox::information(NULL, tr("Path"), tr("Save success!"));
        } else {
            QMessageBox::information(NULL, tr("Path"), tr("Save Failure......"));
        }
    }
    delete fd;
}


//历史回退
void UserInput::back_history() {
    his->back_once(this);
    generate_graph_if_isGenerating();
    update();
}


//历史前进
void UserInput::foward_history() {
    his->forward_once(this);
    generate_graph_if_isGenerating();
    update();
}

UserInput::~UserInput(){}
