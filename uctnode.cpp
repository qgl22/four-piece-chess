#include "uctnode.h"
#include "uct.h"
#include "Judge.h"
#include <math.h>
#include <limits>

UCTNODE::UCTNODE(int x,int y,int player, UCTNODE* parent){
    this->x = x;
    this->y = y;
    this->player = player;
    this->parent = parent;
    this->expand_num = 0;
    this->num = 0;
    this->has_res = -1;
    for(int i = 0; i < MAX_N; i++){
        if(UCT::currentTop[i] > 0){
            expand_nodes[expand_num++] = i;
        }
        child[i] = nullptr;
    }
}

UCTNODE::~UCTNODE(){
    for(int i = 0; i < expand_num; i++){
        delete child[i];
    }
}

void UCTNODE::backup(float profit){
     UCTNODE* tmp = this;
     while(tmp != nullptr){
         tmp->Profit += profit;
         profit = 1 - profit;
         tmp->N++;
         tmp = tmp->parent;
     }
}
UCTNODE* UCTNODE::expand(){
    int child = rand() % expand_num;
    int y = expand_nodes[child];
    int x = --UCT::currentTop[y];
    UCT::currentBoard[x][y] = 3 - this->player;

    if (UCT::currentTop[y] == UCT::noX + 1 && y == UCT::noY) {
        UCT::currentTop[y] --;
    }
    
    this->child[y] = new UCTNODE(x, y, 3 - player, this);

    this->expand_num--;
    this->num++;
    int tmp = expand_nodes[child];
    expand_nodes[child] = expand_nodes[expand_num];
    expand_nodes[expand_num] = tmp;

    return this->child[y];
}

bool UCTNODE::is_end(){
    if(this->parent == nullptr) return -1;
    if(this->has_res != -1){
        return this->has_res;
    }
    if(this->player == 1){
        return this->has_res = ((userWin(this->x, this->y, UCT::M, UCT::N, UCT::currentBoard)) ||(this->expand_num == 0));
    }else{
        return this->has_res = ((machineWin(this->x, this->y, UCT::M, UCT::N, UCT::currentBoard)) ||(this->expand_num == 0));
    }
}

UCTNODE* UCTNODE::bestchild(float coef){
    UCTNODE* best = nullptr;
    float max = std::numeric_limits<float>::lowest();
    int bestY = -1;
    for(int i = this->expand_num + this->num; i > this->expand_num; i--){
        float tmp = child[expand_nodes[i]]->Profit / child[expand_nodes[i]]->N + coef * sqrt(2 * log(N) / child[expand_nodes[i]]->N);
        if(tmp > max || (tmp == max && child[expand_nodes[i]]->is_end())){
            max = tmp;
            best = child[expand_nodes[i]];
            bestY = expand_nodes[i];
        }
    }

    if(coef != 0.0){
        UCT::currentBoard[--UCT::currentTop[bestY]][bestY] = 3 - best->player;
        if (UCT::currentTop[bestY] == UCT::noX + 1 && bestY == UCT::noY) {
            UCT::currentTop[bestY] --;
        }
    }
    return best;
}