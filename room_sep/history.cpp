#include "history.h"
#include <iostream>

//构造函数，构造的时候令其中含有一个空记录
history::history() {
    cur_pos = 0;                        //初始化为0
    undo_times = 0;                     //初始化为0
    his_node * temp = new his_node();   //新建一个节点
    record.push_back(temp);             //加入时间线中
}

//添加一个历史记录
void history::add_his(const UserInput *user) {
    cout<<"In add history"<<endl;                   //调试语句
    undo_times = 0;                                 //一旦添加了历史记录，那么undo的次数就要清零
    his_node * temp = new his_node();               //利用当前数据创建一个临时节点，之后会将这个temp节点加入到时间线record中
    temp->aera = user->area;                        //这三行将userinput中的数据都复制到temp节点中
    temp->circles = user->circles;
    temp->is_using = user->is_using;
    for (int i = 0; i < 100; i++) {                 //将边也复制进去
        for (int j = 0; j < 100; j++) {
            temp->lines[i][j] = user->lines[i][j];
        }
    }
    cur_pos++;                                      //当前记录前进一格
    if (cur_pos == record.size()) {                 //如果此时超出了record的容量，就需要push_back进去
        cout<<"Push Back"<<endl;                    //调试语句
        record.push_back(temp);
    }
    else {                                          //如果并未超出record目前的容量，只需要覆盖这个历史即可
        cout<<"Insert Cover"<<endl;                 //调试语句
        delete record[cur_pos];
        record[cur_pos] = temp;
    }
}

//回退一个历史记录
void history::back_once(UserInput *user) {
    if (!can_undo()) return;                        //不能undo，直接返回
    cout<<"Undoing..."<<endl;                       //调试语句
    cur_pos--;                                      //回退一步
    user->circles = record[cur_pos]->circles;       //这三行将历史记录中的数据全部复制到主界面中
    user->is_using = record[cur_pos]->is_using;
    user->area = record[cur_pos]->aera;
    for (int i = 0; i < 100; i++) {                 //边的信息也是如此
        for (int j = 0; j < 100; j++) {
            user->lines[i][j] = record[cur_pos]->lines[i][j];
        }
    }
    undo_times++;                                   //记录连续undo的次数
}

//前进一个历史记录
void history::forward_once(UserInput *user) {
    if (!can_redo()) return;                        //不能redo，直接return
    cout<<"Redoing..."<<endl;                       //调试语句
    cur_pos++;                                      //历史记录前进
    undo_times--;                                   //所以undo次数减少
    user->circles = record[cur_pos]->circles;       //之后均是将历史记录中的数据全部复制到主界面中
    user->is_using = record[cur_pos]->is_using;
    user->area = record[cur_pos]->aera;
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            user->lines[i][j] = record[cur_pos]->lines[i][j];
        }
    }
}

bool history::can_undo() {
    return cur_pos > 0;
}

bool history::can_redo() {
    return undo_times > 0;
}

