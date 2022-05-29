#include "../indexTree.h"

int main (){
    idxbtree idxtree = yaidxbcreate();
    struct singlepair sg1 = {5,1};
    struct singlepair sg2 = {5,33};

    struct singlepair sg3 = {7, 2};
    struct singlepair sg4 = {7, 6};

    struct singlepair sg5 = {9, 1};

    struct singlepair sg6 = {6, 9};

    struct singlepair sg7 = {10, 77};

    yaidxbinsert(idxtree, sg1);
    yaidxbinsert(idxtree,sg2);
    yaidxbinsert(idxtree,sg3);
    yaidxbinsert(idxtree,sg4);
    yaidxbinsert(idxtree,sg5);
    yaidxbinsert(idxtree,sg6);
    yaidxbinsert(idxtree,sg7);

    yaidxbdelete(idxtree,sg4);
    yaidxbdelete(idxtree, sg3);
    
    dump_tree(idxtree);

    
}