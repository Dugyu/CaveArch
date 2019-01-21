#ifndef MYPAINTERWIDGET_H
#define MYPAINTERWIDGET_H

#include <QWidget>
#include <QPoint>
#include <vector>
#include <queue>
#include <QString>
#include <utility>
#include <QPushButton>

using namespace std;

struct myLine {
    QPoint startPnt;
    QPoint endPnt;
};

struct Point {
    Point(){}
    Point(int a, int b): x(a), y(b){}
    int x, y;
};



class MyPainterWidget: public QWidget {
public:
    MyPainterWidget(QWidget* parent);
    ~MyPainterWidget();

    //继承
    void paintEvent(QPaintEvent * p);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent * k);

    QPoint startPnt;
    QPoint endPnt;
    bool isPressed;
    Point left_corner, right_corner;

    vector<myLine*> lines;
    vector<int> rooms;
    vector<pair<Point, QString>> labels;
    QPushButton * button;
};


pair<vector<int>, vector<int> >
divide(vector<int> &rooms, QPainter &painter, Point st, Point ed, queue<Point> &points, MyPainterWidget & mywidget);

#endif // MYPAINTERWIDGET_H
