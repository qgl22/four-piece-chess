#include "uct.h"
#include "uctnode.h"
#include "Judge.h"
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>

int UCT::M = 0;
int UCT::N = 0;
int UCT::noX = 0;
int UCT::noY = 0;
int UCT::currentTop[MAX_N];
int** UCT::currentBoard = nullptr;

void UCT::build(const int ** oldboard,const int * oldtop,int*x,int*y){

    struct timeval begin, now;
    gettimeofday(&begin, NULL);
    long us = begin.tv_sec * 1000000 + begin.tv_usec;
    long cur_us = us;
    long aim_us = us + 2900000;
    this->root = new UCTNODE(-1, -1, OTHER, nullptr);

    while(cur_us < aim_us){
        for (int j = 0;j < M;j++) {
            for (int k = 0;k < N;k++) {
                currentBoard[j][k] = oldboard[j][k];
            }
        }
        memcpy(currentTop, oldtop, N * sizeof(int));

        UCTNODE* nd =  select(this->root);
        float profit = simulate(nd);
        nd->backup(profit);

        gettimeofday(&now, NULL);
        cur_us = now.tv_sec * 1000000 + now.tv_usec;
    }

    UCTNODE* best = this->root->bestchild(0.0);
    *x = best->x;
    *y = best->y;
    delete this->root;
}

UCTNODE* UCT::select(UCTNODE* node){
    while(node->is_end() != 0 ){
        if(node->expand_num > 0){
            return node->expand();
        }else{
            node = node->bestchild(1.0);
        }
    }
    return node;
}

float UCT::simulate(UCTNODE* node){
    int not_begin = false;
    if (isTie(UCT::N, UCT::currentTop)) {
        return 0.5;
    }else if(machineWin(node->x, node->y, UCT::M, UCT::N, UCT::currentBoard)){
        if(node->player == SELF) return 1.0;
        else return 0.0;
    }else if(userWin(node->x, node->y, UCT::M, UCT::N, UCT::currentBoard)){
        if(node->player == SELF) return 0.0;
        else return 1.0;
    }
    
    int currentPlayer = 3 - node->player;
    int winnerPlayer = 0;
    while(true){
        if (not_begin && isTie(UCT::N, UCT::currentTop)) {
            not_begin = true;
            return 0.5;
        }

        int deadnum = 0;
        int deadX = 0, deadY = 0;
        for (int y = 0;y < N;y++) {
            if (currentTop[y] < 1) {
                continue;
            }
            int x = currentTop[y] - 1;
            currentBoard[x][y] = currentPlayer;
            if (currentPlayer == SELF && machineWin(x, y, UCT::M, UCT::N, UCT::currentBoard)) {
                winnerPlayer = SELF;
                return winnerPlayer == node->player ? 1.0 : 0.0;
            } else if (currentPlayer == OTHER && userWin(x, y, UCT::M, UCT::N, UCT::currentBoard)) {
                winnerPlayer = OTHER;
                return winnerPlayer == node->player ? 1.0 : 0.0;
            }

            currentBoard[x][y] = 3 - currentPlayer;
            if (currentPlayer == SELF && machineWin(x, y, UCT::M, UCT::N, UCT::currentBoard)) {
                deadnum ++;
                deadX = x;
                deadY = y;
            } else if (currentPlayer == OTHER && userWin(x, y, UCT::M, UCT::N, UCT::currentBoard)) {
                deadnum ++;
                deadX = x;
                deadY = y;
            }
            currentBoard[x][y] = 0;
        }

        if (deadnum >= 2) {
            winnerPlayer = 3 - currentPlayer;
            return winnerPlayer == node->player ? 1.0 : 0.0;
        }else if (deadnum == 1) {
            currentTop[deadY] --;
            if (deadX - 1 == noX && deadY == noY) {
                currentTop[deadY]--;
            }
            currentBoard[deadX][deadY] = currentPlayer;
            currentPlayer = 3 - currentPlayer;
            continue;
        }else {
            int y = rand() % N;
            while (currentTop[y] == 0) {
                y = rand() % N;
            }
            int x = --currentTop[y];

            if (x - 1 == noX && y == noY) {
                currentTop[y]--;
            }
            currentBoard[x][y] = currentPlayer;

            if (x > 0 && x - 1 != noX) {
                currentBoard[x-1][y] = 3 - currentPlayer;
                if (currentPlayer == SELF && userWin(x-1, y, UCT::M, UCT::N, UCT::currentBoard)) {
                    winnerPlayer = OTHER;
                    return winnerPlayer == node->player ? 1.0 : 0.0;
                } else if (currentPlayer == OTHER && machineWin(x-1, y, UCT::M, UCT::N, UCT::currentBoard)) {
                    winnerPlayer = SELF;
                    return winnerPlayer == node->player ? 1.0 : 0.0;
                }
                currentBoard[x-1][y] = 0;
            }
            currentPlayer = 3 - currentPlayer;
        }
    }
}
