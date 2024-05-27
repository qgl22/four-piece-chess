#pragma once
#include "uct.h"


//TODO:UCT中的bestchild函数，backup函数用于回溯，expand函数用于扩展，is_end函数用于检测是否结束
class UCTNODE{
    public:
        int x,y;
        int player;
        int expand_num;  //可以扩展的节点数
        int num; //已经扩展的节点数
        int expand_nodes[MAX_N];
        float Profit; //目前为止的收益
        int N; //场次
        int has_res;
        UCTNODE* parent;
        UCTNODE* child[MAX_N];
        UCTNODE(int x,int y,int player, UCTNODE* parent);
        ~UCTNODE();
        UCTNODE* bestchild(float coef);
        void backup(float profit);
        UCTNODE* expand();
        bool is_end();
};