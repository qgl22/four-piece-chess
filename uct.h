#pragma once

#define MAX_M 12
#define MAX_N 12
#define OTHER 1
#define SELF 2


class UCTNODE;
class UCT{
    public:
        static int M, N;
        static int noX, noY;
        static int currentTop[MAX_N];
        static int **currentBoard;
        void build(const int ** oldboard,const int * oldtop,int*x,int* y); //UCT的构建函数
        UCTNODE* select(UCTNODE* node); //选取节点 
        float simulate(UCTNODE* node); //随机模拟得到收益
        static void initialize(int M, int N, int noX, int noY) {
            UCT::M = M;
            UCT::N = N;
            UCT::noX = noX;
            UCT::noY = noY;
            UCT::currentBoard = new int *[M];
            for (int i = 0;i < M;i++) {
                UCT::currentBoard[i] = new int[N];
            }
        }
        static void cleanupBoard() {
            for (int i = 0;i < M;i++) {
                delete [] UCT::currentBoard[i];
            }
            delete [] UCT::currentBoard;
        }
    private:
        UCTNODE* root;
        
};
