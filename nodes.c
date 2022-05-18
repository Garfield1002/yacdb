#include "nodes.h"

btree bcreate(void){
  btree t;
  t = malloc(sizeof(*t));
  t->is_leaf = true;
  t->nb_keys = 0;
  return t;
}

struct data *yabsearch(btree t, int searched_key) {
  //We have to find out if the searched_key is among the keys of the current node. If not, we have to find the only 'sub-tree' that can contain the key.
  //As the keys are sorted, we can use dichotomical search instead of classic search

  int min = -1;
  int max = t->nb_keys;

  while (max-min >1){
    int cur_idx = (max+min)/2;
    //if the key is found out, retrieve the corresponding value
    int cur_key = t->key_val[cur_idx].key;
    if (cur_key == searched_key){
      return t->key_val[cur_idx].value;
    }
    else if(cur_key < searched_key){
      min = cur_key;
    }
    else{
      max = cur_key;
    }
  }
  //if we get to this point, that means we haven't found the target in the current node, we have to search into the correct 'subtree'
  //At this point, min and max are the virtual indexes (because min can be equal to -1 and max equal to nb_keys) of the two keys between which the searched key is.
  
  if (t->is_leaf){
    return 0;
  }

  int child_idx = max;
  return yabsearch(t->children[child_idx], searched_key);
  
}
/*
void yabinsert(btree t, struct key_value key_val){
  //we start from the root, we check if we came to a leaf
  struct node *curr_node = t;
  while (curr_node->is_leaf == false) {
     
  }  
}
*/
int main(){
  btree ttest = bcreate();
  struct data *tdata = yabsearch(ttest, 33);
}

