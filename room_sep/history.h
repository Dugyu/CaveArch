#ifndef HISTORY_H
#define HISTORY_H
#include <iostream>
#include <vector>
#include <QPoint>
#include "userinput.h"
using namespace std;
struct his_node;
class UserInput;

class history {
public:
    history();                              //构造函数
    ~history();                             //析构函数
    int cur_pos;                            //当前历史位置
    int undo_times;                         //记录目前为止连续undo的次数，这是判断能够redo的关键
    vector<his_node *> record;              //记录的所有历史记录，可以理解为时间线，cur_pos在其中移动
    void add_his(const UserInput *user);    //添加一个历史记录
    void back_once(UserInput *user);        //回退一个历史记录
    void forward_once(UserInput * User);    //前进一个历史记录
    bool can_undo();                        //判断能够undo
    bool can_redo();                        //判断能够redo
};

//his_node是history_node的缩写，保存了一个时刻的所有信息
//一个his_node能够完全复原当时的原图，对偶图则不作改变
struct his_node {
    his_node() {                            //构造函数，主要是给边的矩阵分配空间
        lines = new int *[100]();
        for (int i = 0; i < 100; i++) {
            lines[i] = new int[100]();
        }
    }

    ~his_node() {                           //析构函数，主要也就是删除那个分配的数组
        cout<<"deleteing his_node"<<endl;   //调试语句
        for (int i = 0; i < 100; i++) {
            delete lines[i];
        }
        delete lines;
    }

    vector<QPoint> circles;                 //保存原图中点的坐标
    vector<bool> is_using;                  //保存改点是否在使用 todo:能否不用？
    vector<int> aera;                       //保存原图中每个点的面积
    int **lines;                            //保存原图中的边
};

#endif // HISTORY_H
