#include "nodes.h"


// yabcreate creates a new empty tree (pointer to a node with no key)
btree yabcreate(void){
  btree t;
  t = malloc(sizeof(*t));
  t->is_leaf = true;
  t->nb_keys = 0;
  return t;
}


// bcreate_start creates a tree with an initial key in the root node
btree bcreate_start(struct key_value key_v) {
  btree t;
  t = malloc(sizeof(*t));
  t->is_leaf = false;
  t->nb_keys = 1;
  t->key_val[0] = key_v;
  btree c1 = yabcreate();
  btree c2 = yabcreate();
  t->children[0] = c1;
  t->children[1] = c2;
  return t;
}


// node_to_string dumps a node (number of keys and its keys)
void node_to_string(struct node* nod){
  char string[1024];
  printf("[ %d |", nod->nb_keys);
  for (int i=0; i<nod->nb_keys;i++){
    printf(" %d |", nod->key_val[i].key); 
  }
  printf("\n");
}


// dump_tree dumps the node and its direct children if there are any
void dump_tree(btree t){
  printf("ROOT-----------------\n");
  node_to_string(t);
  if (!(t->is_leaf)){
    for(int i=0; i<=t->nb_keys;i++){
      printf("LEVEL 1---------------------\n");
      node_to_string(t->children[i]);
    }
  }
}


//yabgetall retrieves every data stored in the tree which corresponds to all the data in the leaf nodes as it is a B+Tree, it takes the tree as an argument and an empty dataaray that will be populated
void yabgetall(btree t, dataarray *allfound){
  int current_size;
  
  //if the current node is a leaf, then we populate allfound with all the data in the node
  if(t->is_leaf){
    for (int k=0; k<t->nb_keys;k++){
      current_size = allfound->size;
      allfound->data[current_size] = t->key_val[k].value;
      memcpy(&(allfound->data[current_size]), &(t->key_val[k].value), sizeof(t->key_val[k].value));
      allfound->size++;
    }
  }
  //if not, we look into each of the subtree formed by the children
  else {
    for (int ch=0;ch<=t->nb_keys;ch++){
      yabgetall(t->children[ch], allfound); 
    }
  }
}


//yabsearch takes the tree and a key as arguments and retrieves a pointer to the data corresponding to the key if there is any, otherwise it returns 0
struct data *yabsearch(btree t, int searched_key) {
  //We have to find out if the searched_key is among the keys of the current node. If not, we have to find the only 'sub-tree' that can contain the key.
  //As the keys are sorted, we can use dichotomical search instead of classic search
  int min = -1;
  int max = t->nb_keys;

  while (max-min >1){
    int cur_idx = (max+min)/2;
    int cur_key = t->key_val[cur_idx].key;
    //if the key is found out and the current node is a leaf, retrieve the corresponding data. Otherwise, we keep searching
    if (cur_key == searched_key){
      if (t->is_leaf){
        return t->key_val[cur_idx].value;
      }
      else {
        max = cur_idx;
	      break;
      }
    }
    else if(cur_key < searched_key){
      min = cur_idx;
    }
    else{
      max = cur_idx;
    }
  }
  //if we get to this point, that means we haven't found the target in the current node, we have to search into the correct 'subtree'
  //At this point, min and max are the virtual indexes (because min can be equal to -1 or max equal to nb_keys) of the two keys between which the searched key is.
  
  //if we already are in a leaf node, that means, the searched key is not in the tree
  if (t->is_leaf){
    return 0;
  }
  //else, we look into the correct subtree
  int child_idx = max;
  return yabsearch(t->children[child_idx], searched_key);
 
}


//splitChild takes a node as an argument, an index and a key. It splits the node's children designated by the index in two and returns one of the node based on the key
//We have to split whenever a node reaches its maximum amount of keys.
struct node *splitChild(struct node *old_parent, int idx_child_to_split, int searched_key){
  printf("DEBUT SPLITCHILD\n");
  struct node *correct_child = old_parent->children[idx_child_to_split];
  //First, we have to find out the key_value pair that's around the middle of the pairs list of the children and its index.
  int mid_idx = (correct_child->nb_keys)/2;
  mid_idx--;
  struct key_value pair_to_up = correct_child->key_val[mid_idx];

  //we create the new nodes product by the splitting
  struct node* new_left_node;
  struct node* new_right_node;
  new_left_node = malloc(sizeof(*new_left_node)); 
  new_right_node = malloc(sizeof(*new_right_node)); 
  //We initialize the nb of key_val pairs which will be incremented as we fill up the lists
  new_left_node->nb_keys=0;
  new_right_node->nb_keys=0;
  
  //If the old child was a leaf then the new ones are too, otherwise they're not
  new_left_node->is_leaf = correct_child->is_leaf;
  new_right_node->is_leaf = correct_child->is_leaf;

  //We start by filling the left node
  int i;
  for (i=0;i<=mid_idx;i++){
    new_left_node->nb_keys ++;
    new_left_node->key_val[i] = correct_child->key_val[i];
    if (!(new_left_node->is_leaf)){
      new_left_node->children[i] = correct_child->children[i];
    }
  }
  //Not forget to add the rightmost child which will be a brand new node
  if(!new_left_node->is_leaf){
  struct node *last_node;
  last_node =malloc(sizeof(*last_node));
  last_node->nb_keys=0;
  last_node->is_leaf=new_left_node->children[0]->is_leaf;
  new_left_node->children[i] = last_node;
  }
  
  int tmp2;
  //Now, let's fill the right one
  for (int i=mid_idx+1;i<correct_child->nb_keys;i++){
    new_right_node->nb_keys ++;
    new_right_node->key_val[i-mid_idx-1] = correct_child->key_val[i];
    if (!(new_right_node->is_leaf)){
      new_right_node->children[i-mid_idx-1] = correct_child->children[i];
    }
    tmp2=i-mid_idx-1;
  }


  //Not forget to add the rightmost child
  if (!(new_right_node->is_leaf)){
    new_right_node->children[tmp2+1] = correct_child->children[correct_child->nb_keys];
  } 
  
  //We store the old lists of children and keys of the node as we are going to modify them
  struct key_value old_pairs[ORDER-1];
  struct node *old_children[ORDER];
  memcpy(&old_pairs, &(old_parent->key_val), sizeof(old_parent->key_val));
  memcpy(&old_children, &(old_parent->children), sizeof(old_parent->children));
  
  //We bring the middle key up. We put the new nodes at the right places
  old_parent->key_val[idx_child_to_split] = pair_to_up;
  old_parent->children[idx_child_to_split] = new_left_node;
  old_parent->children[idx_child_to_split+1] = new_right_node;
  old_parent->nb_keys++;
  
  for (int new_idx = idx_child_to_split+1; new_idx < old_parent->nb_keys;new_idx++){
    old_parent->key_val[new_idx] = old_pairs[new_idx-1];
    old_parent->children[new_idx+1] = old_children[new_idx];
  }

  printf("FIN SPLITCHILD\n");

  //As the childSplit was called during a traversal, we returns the correct child
  if (pair_to_up.key < searched_key){
    return old_parent->children[idx_child_to_split+1];
  }
  else {
    return old_parent->children[idx_child_to_split];
  }

}


//yabinsert inserts a key-value pair into the right node
int yabinsert(btree t, struct key_value key_val){
  printf("DEBUT INSERT\n"); 

  //if the root is already full, a childSplit on the roor is needed
  if (t->nb_keys >= ORDER-1){
    //As the root doesn't have a parent, we create one which only child will be the full root
    struct node *virt_racine;
    virt_racine = malloc(sizeof(*virt_racine));
    virt_racine ->is_leaf=false;
    virt_racine ->nb_keys=0;
    virt_racine ->children[0] = t;
    //We can call splitChild on the new root
    splitChild(virt_racine, 0, 69);
    //The tree has to point towards the new root
    *t = *virt_racine;
  }
  //we start from the root, we check if we came to a leaf
  struct node *curr_node = t;
  //The value of the key to insert
  int searched_key = key_val.key;
  
  //A traversal search as usual
  while (curr_node->is_leaf == false) {
    int min = -1;
    int max = curr_node->nb_keys;
    while (max-min >1){
      int cur_idx = (max+min)/2;
      int cur_key = curr_node->key_val[cur_idx].key;
      if(cur_key <= searched_key){
        min = cur_idx;
      }
      else{
        max = cur_idx;
      }
    }
    //At this point, we know the index of the children which has to be the next node to look in
    struct node *correct_child = curr_node->children[max];
    //if the next node is full, we have to split it in two nodes before going down to the correct node
    if (correct_child->nb_keys < ORDER-1){
      curr_node = correct_child;
      continue;
    }
    else {
      curr_node = splitChild(curr_node, max,searched_key);  
    } 
  }
  //We have arrived on the only node that could countain the key we want to insert, we have to check if the keys is already present
  bool already = false;
  for (int j=0;j<curr_node->nb_keys;j++){
    already = already | curr_node->key_val[j].key == key_val.key;
  }
  //if it doesn't already exist, we have to insert it in the correct index
  if (!already){
    struct key_value new_key_val[ORDER-1];
    //we look through all the keys which are already sorted and populate a new array of keys including the one to insert.
    // We don't have to worry about the children array as the leaf nodes don't have children
    int i;
    for (i=0;i< curr_node->nb_keys;i++){
      if (curr_node->key_val[i].key > key_val.key){
        break;
      }
      new_key_val[i]=curr_node->key_val[i];
    }

    new_key_val[i] =key_val;
    for (int j=i+1;j< curr_node->nb_keys+1;j++){
      new_key_val[j] = curr_node -> key_val[j-1];
    }
    //we replace the node's keys array by the new one
    memcpy(&(curr_node->key_val),&new_key_val,sizeof(new_key_val));
    //not forget to increment the node's number of key attribute
    curr_node->nb_keys++;
  }

  else {
    printf("Insertion of key failed, the key was already existing\n");
  }

  printf("FIN INSERT\n");
  return 1;
}


//yabdelete performs a traversal search as yabinsert in order to delete a key-value pair
int yabdelete(btree t, int key){
  struct node *curr_node = t;
  while (curr_node->is_leaf == false) {
    int min = -1;
    int max = curr_node->nb_keys;
    while (max-min >1){
      int cur_idx = (max+min)/2;
      int cur_key = curr_node->key_val[cur_idx].key;
      if(cur_key < key){
        min = cur_idx;
      }
      else{
        max = cur_idx;
      }
    }
    //At this point, we know the index of the children which has to be the next node to look in
    struct node *correct_child = curr_node->children[max];
    curr_node = correct_child;

  }
  //As for yabinsert, we populate the keys-value array surpressing the right one
  struct key_value new_key_val[ORDER-1];
  bool found=false;
  for (int i=0;i<curr_node->nb_keys;i++){
    if (curr_node->key_val[i].key !=key){
      new_key_val[i-found]=curr_node->key_val[i];
    }
    else {
      found=true;
    }
  }
  if (!found) {
    printf("Deletion failed, key not found ! \n");
    return 1;
  }
  else {
    curr_node->nb_keys--;
    memcpy(&(curr_node->key_val),&new_key_val,sizeof(new_key_val));
  }
  return 1;

}
