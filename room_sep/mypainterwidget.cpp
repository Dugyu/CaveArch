#include "mypainterwidget.h"
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <fstream>
#include <utility>
#include <queue>
#include <random>
#include <algorithm>
#include <math.h>
#include <QPushButton>
#include <time.h>
#include <QVBoxLayout>

using namespace std;

MyPainterWidget::MyPainterWidget(QWidget * parent) : QWidget(parent) {
    setMinimumSize(720, 380);
    setMaximumSize(720, 380);
    this->setMouseTracking(true);
    this->isPressed = false;
    fstream in("/Users/huanghuangsunyang/Desktop/1.txt");
    int n;
    in>>n;
    for (int i = 0; i < n; i++) {
        int temp;
        in>>temp;
        rooms.push_back(temp);
    }
    left_corner = Point(60, 40);
    right_corner = Point(660, 340);
}

MyPainterWidget::~MyPainterWidget() {}

void MyPainterWidget::paintEvent(QPaintEvent *p) {
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::darkCyan);
    pen.setWidth(3);
    painter.setPen(pen);
    for(int i = 0; i < lines.size(); i++){
        myLine* pLine = lines[i];
        painter.drawLine(pLine->startPnt,pLine->endPnt);
    }
    painter.drawRect(left_corner.x, left_corner.y, right_corner.x - left_corner.x,
                     right_corner.y - left_corner.y);
    queue<vector<int> > que;
    queue<Point> points;
    points.push(left_corner);
    points.push(right_corner);
    que.push(rooms);
    while (!que.empty()) {
        Point a = points.front();
        points.pop();
        Point b = points.front();
        points.pop();
        vector<int> temp = que.front();
        que.pop();
        pair<vector<int>, vector<int>> p = divide(temp, painter, a, b, points, *this);
        if (p.first.size() > 1) que.push(p.first);
        if (p.second.size() > 1) que.push(p.second);
    }

    for (int i = 0; i < labels.size(); i++) {
        painter.drawText(labels[i].first.x, labels[i].first.y, labels[i].second);
    }
    labels.clear();
}

void MyPainterWidget::mousePressEvent(QMouseEvent *e) {
    setCursor(Qt::PointingHandCursor);
    startPnt = e->pos();
    endPnt = e->pos();
    this->isPressed = true;
    update();
    //QString msg ="("+QString::number(e->x())+","+QString::number(e->y())+")";
    //QMessageBox::warning(this,tr("Warning"),msg,QMessageBox::Ok);
}

void MyPainterWidget::mouseMoveEvent(QMouseEvent *e) {
    if(this->isPressed){
        endPnt = e->pos();
        if (abs(endPnt.rx() - left_corner.x) < 20) {
            left_corner.x = endPnt.rx();
        } else if (abs(endPnt.rx() - right_corner.x) < 20) {
            right_corner.x = endPnt.rx();
        }
        if (abs(endPnt.ry() - left_corner.y) < 20) {
            left_corner.y = endPnt.ry();
        } else if (abs(endPnt.ry() - right_corner.y) < 20) {
            right_corner.y = endPnt.ry();
        }
        if (abs(endPnt.ry() - left_corner.y/2 - right_corner.y/2) < 20 &&
            abs(endPnt.rx() - left_corner.x/2 - right_corner.x/2) < 20) {
            left_corner.y += endPnt.ry() - startPnt.ry();
            left_corner.x += endPnt.rx() - startPnt.rx();
            right_corner.y += endPnt.ry() - startPnt.ry();
            right_corner.x += endPnt.rx() - startPnt.rx();
        }

        myLine* line = new myLine;  //put the new line into vector
        line->startPnt = startPnt;
        line->endPnt = endPnt;
        //this->lines.push_back(line);

        update();                                    //repainter，call paintEvent
        startPnt = endPnt;
    }
}

void MyPainterWidget::mouseReleaseEvent(QMouseEvent *e) {
    setCursor(Qt::ArrowCursor);
    this->isPressed = false;
}

void MyPainterWidget::keyPressEvent(QKeyEvent *k) {
    update();
}

Point middles(Point a, Point b) {
    return Point((a.x + b.x) / 2, (a.y + b.y) / 2);
}

pair<vector<int>, vector<int> >
divide(vector<int> &rooms, QPainter &painter, Point st, Point ed, queue<Point> &points,
      MyPainterWidget & mywidget) {
    int total_aera = accumulate(rooms.begin(), rooms.end(), 0);
    int target = total_aera / 2;
    vector<vector<int> > matrix(target + 1, vector<int>(rooms.size(), 0));
    vector<vector<int> > previous(target + 1, vector<int>(rooms.size(), 0));
    for (int j = 0; j < rooms.size(); j++) {
        matrix[0][j] = 0;
        previous[0][j] = 0;
    }
    for (int i = 0; i < target + 1; i++) {
        if (i >= rooms[0]) {
            matrix[i][0] = i - rooms[0];
            previous[i][0] = 1;
        } else {
            matrix[i][0] = i;
        }
    }
    //动态规划求解
    for (int i = 1; i < target + 1; i++) {
        for (int j = 1; j < rooms.size(); j++) {
            if (rooms[j] > i) {
                matrix[i][j] = matrix[i][j - 1];
                previous[i][j] = 0;
            } else {
                if (matrix[i][j-1] < matrix[i - rooms[j]][j - 1]) {
                    matrix[i][j] = matrix[i][j-1];
                    previous[i][j] = 0;
                } else {
                    matrix[i][j] = matrix[i - rooms[j]][j - 1];
                    previous[i][j] = 1;
                }
            }
        }
    }
    //通过记录回溯得到一半集合
    vector<int> left_part, right_part;
    int y = int(rooms.size()) - 1;
    int x = target;
    while (x >= 0 && y >= 0) {
        if (previous[x][y] == 1) {
            left_part.push_back(y);
            x -= rooms[y];
            y --;
        } else {
            y--;
        }
    }
    //获取另一半
    int * temp_count = new int[rooms.size()]();
    for (int i = 0; i < left_part.size(); i++) {
        temp_count[left_part[i]] = 1;
    }
    for (int i = 0; i < rooms.size(); i++) {
        if (temp_count[i] != 1) {
            right_part.push_back(i);
        }
    }
    transform(left_part.begin(), left_part.end(), left_part.begin(),
                   [&rooms](int i){return rooms[i];});
    transform(right_part.begin(), right_part.end(), right_part.begin(),
                   [&rooms](int i){return rooms[i];});

    //随机交换两部分
    std::random_device rd;
    std::mt19937 mt(rd());
    if (mt() % 2 == 1) {
        swap(left_part, right_part);
    }

    //求出两部分的比例，开始绘图
    int left_aera = accumulate(left_part.begin(), left_part.end(), 0);
    double rate = left_aera * 1.0 / total_aera;
    if (ed.x - st.x > ed.y - st.y) {
        Point top_mid(st.x + (ed.x - st.x) * rate, st.y),
              bottom_mid(st.x + (ed.x - st.x) * rate, ed.y);
        painter.drawLine(top_mid.x, top_mid.y, bottom_mid.x, bottom_mid.y);
        //为之后的两次分割作准备
        if (left_part.size() > 1) {
            points.push(st);
            points.push(bottom_mid);
        } else {
            mywidget.labels.push_back(make_pair(middles(st, bottom_mid),
                                                QString::number(left_part[0])));
        }
        if (right_part.size() > 1) {
            points.push(top_mid);
            points.push(ed);
        } else {
            mywidget.labels.push_back(make_pair(middles(top_mid, ed),
                                                QString::number(right_part[0])));
        }
    } else {
        Point left_mid(st.x, st.y + (ed.y - st.y) * rate),
              right_mid(ed.x, st.y + (ed.y - st.y) * rate);
        painter.drawLine(left_mid.x, left_mid.y, right_mid.x, right_mid.y);
        //为之后的两次分割作准备
        if (left_part.size() > 1) {
            points.push(st);
            points.push(right_mid);
        } else {
            mywidget.labels.push_back(make_pair(middles(st, right_mid),
                                                QString::number(left_part[0])));
        }
        if (right_part.size() > 1) {
            points.push(left_mid);
            points.push(ed);
        } else {
            mywidget.labels.push_back(make_pair(middles(left_mid, ed),
                                                QString::number(right_part[0])));
        }
    }
    return make_pair(left_part, right_part);
}
