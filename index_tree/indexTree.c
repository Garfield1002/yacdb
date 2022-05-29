#include "indexTree.h"


// yabcreate creates a new empty tree (pointer to a node with no key)
idxbtree yaidxbcreate(void)
{
  idxbtree t;
  t = malloc(sizeof(*t));
  t->is_leaf = true;
  t->nb_keys = 0;
  return t;
}


// bcreate_start creates a tree with an initial key in the root node
idxbtree idxbcreate_start(struct idxkey_value key_v) 
{
  idxbtree t;
  t = malloc(sizeof(*t));
  t->is_leaf = false;
  t->nb_keys = 1;
  t->key_val[0] = key_v;
  idxbtree c1 = yaidxbcreate();
  idxbtree c2 = yaidxbcreate();
  t->children[0] = c1;
  t->children[1] = c2;
  return t;
}


// node_to_string dumps a node (number of keys and its keys)
void idxnode_to_string(struct idxnode* nod)
{
  char string[1024];
  printf("[ %d ", nod->nb_keys);

  for (int i=0; i<nod->nb_keys;i++)
  {
    int sz = nod->key_val[i].value->size;
    printf("| %d (%d) :", nod->key_val[i].idxkey,sz);

    for (int j=0;j<sz;j++){
      printf(" %d ", nod->key_val[i].value->keyarray[j]);
    }
  }
  printf("\n");
}


// idxdump_tree dumps the node and its direct children if there are any
void idxdump_tree(idxbtree t)
{
  printf("ROOT-----------------\n");
  idxnode_to_string(t);

  if (!(t->is_leaf))
  {

    for(int i=0; i<=t->nb_keys;i++)
    {
      printf("LEVEL 1---------------------\n");
      idxnode_to_string(t->children[i]);
    }
  }
}


//yabgetall retrieves every data stored in the tree which corresponds to all the data in the leaf nodes as it is a B+Tree, it takes the tree as an argument and an empty dataaray that will be populated
void yaidxbgetall(idxbtree t, idxdataaray *allfound)
{
  int current_size;
  //if the current node is a leaf, then we populate allfound with all the data in the node

  if(t->is_leaf)
  {

    for (int k=0; k<t->nb_keys;k++)
    {
      current_size = allfound->size;
      allfound->data[current_size] = t->key_val[k].value;
      memcpy(&(allfound->data[current_size]), &(t->key_val[k].value), sizeof(t->key_val[k].value));
      allfound->size++;
    }
  }
  //if not, we look into each of the subtree formed by the children

  else 
  {

    for (int ch=0;ch<=t->nb_keys;ch++)
    {
      yaidxbgetall(t->children[ch], allfound); 
    }
  }
}


//yabsearch takes the tree and a key as arguments and retrieves a pointer to the data corresponding to the key if there is any, otherwise it returns 0
struct idxdata *yaidxbsearch(idxbtree t, int searched_key) 
{
  //We have to find out if the searched_key is among the keys of the current node. If not, we have to find the only 'sub-tree' that can contain the key.
  //As the keys are sorted, we can use dichotomical search instead of classic search
  int min = -1;
  int max = t->nb_keys;

  while (max-min >1)
  {
    int cur_idx = (max+min)/2;
    int cur_key = t->key_val[cur_idx].idxkey;
    //if the key is found out and the current node is a leaf, retrieve the corresponding data. Otherwise, we keep searching

    if (cur_key == searched_key)
    {

      if (t->is_leaf)
      {
        return t->key_val[cur_idx].value;
      }

      else 
      {
        max = cur_idx;
	      break;
      }
    }

    else if(cur_key < searched_key)
    {
      min = cur_idx;
    }

    else
    {
      max = cur_idx;
    }
  }
  //if we get to this point, that means we haven't found the target in the current node, we have to search into the correct 'subtree'
  //At this point, min and max are the virtual indexes (because min can be equal to -1 or max equal to nb_keys) of the two keys between which the searched key is.
  
  //if we already are in a leaf node, that means, the searched key is not in the tree

  if (t->is_leaf)
  {
    return 0;
  }
  //else, we look into the correct subtree
  int child_idx = max;
  return yaidxbsearch(t->children[child_idx], searched_key);
 
}


//idxsplitChild takes a node as an argument, an index and a key. It splits the node's children designated by the index in two and returns one of the node based on the key
//We have to split whenever a node reaches its maximum amount of keys.
struct idxnode *idxsplitChild(struct idxnode *old_parent, int idx_child_to_split, int searched_key)
{
  printf("DEBUT idxsplitChild\n");
  struct idxnode *correct_child = old_parent->children[idx_child_to_split];
  //First, we have to find out the key_value pair that's around the middle of the pairs list of the children and its index.
  int mid_idx = (correct_child->nb_keys)/2;
  mid_idx--;
  struct idxkey_value pair_to_up = correct_child->key_val[mid_idx];

  //we create the new nodes product by the splitting
  struct idxnode* new_left_node;
  struct idxnode* new_right_node;
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

  for (i=0;i<=mid_idx;i++)
  {
    new_left_node->nb_keys ++;
    new_left_node->key_val[i] = correct_child->key_val[i];
    
    if (!(new_left_node->is_leaf))
    {
      new_left_node->children[i] = correct_child->children[i];
    }
  }
  //Not forget to add the rightmost child which will be a brand new node

  if(!new_left_node->is_leaf)
  {
  struct idxnode *last_node;
  last_node =malloc(sizeof(*last_node));
  last_node->nb_keys=0;
  last_node->is_leaf=new_left_node->children[0]->is_leaf;
  new_left_node->children[i] = last_node;
  }
  
  int tmp2;
  //Now, let's fill the right one

  for (int i=mid_idx+1;i<correct_child->nb_keys;i++)
  {
    new_right_node->nb_keys ++;
    new_right_node->key_val[i-mid_idx-1] = correct_child->key_val[i];

    if (!(new_right_node->is_leaf)){
      new_right_node->children[i-mid_idx-1] = correct_child->children[i];
    }
    tmp2=i-mid_idx-1;
  }


  //Not forget to add the rightmost child

  if (!(new_right_node->is_leaf))
  {
    new_right_node->children[tmp2+1] = correct_child->children[correct_child->nb_keys];
  } 
  
  //We store the old lists of children and keys of the node as we are going to modify them
  struct idxkey_value old_pairs[ORDER-1];
  struct idxnode *old_children[ORDER];
  memcpy(&old_pairs, &(old_parent->key_val), sizeof(old_parent->key_val));
  memcpy(&old_children, &(old_parent->children), sizeof(old_parent->children));
  
  //We bring the middle key up. We put the new nodes at the right places
  old_parent->key_val[idx_child_to_split] = pair_to_up;
  old_parent->children[idx_child_to_split] = new_left_node;
  old_parent->children[idx_child_to_split+1] = new_right_node;
  old_parent->nb_keys++;
  
  for (int new_idx = idx_child_to_split+1; new_idx < old_parent->nb_keys;new_idx++)
  {
    old_parent->key_val[new_idx] = old_pairs[new_idx-1];
    old_parent->children[new_idx+1] = old_children[new_idx];
  }

  printf("FIN idxsplitChild\n");

  //As the childSplit was called during a traversal, we returns the correct child

  if (pair_to_up.idxkey < searched_key)
  {
    return old_parent->children[idx_child_to_split+1];
  }

  else {
    return old_parent->children[idx_child_to_split];
  }

}


//yabinsert inserts a key-value pair into the right node
int yaidxbinsert(idxbtree t, struct singlepair key_val)
{
  printf("DEBUT INSERT\n"); 

  //if the root is already full, a childSplit on the roor is needed

  if (t->nb_keys >= ORDER-1)
  {
    //As the root doesn't have a parent, we create one which only child will be the full root
    struct idxnode *virt_racine;
    virt_racine = malloc(sizeof(*virt_racine));
    virt_racine ->is_leaf=false;
    virt_racine ->nb_keys=0;
    virt_racine ->children[0] = t;
    //We can call idxsplitChild on the new root
    idxsplitChild(virt_racine, 0, 69);
    //The tree has to point towards the new root
    *t = *virt_racine;
  }
  //we start from the root, we check if we came to a leaf
  struct idxnode *curr_node = t;
  //The value of the key to insert
  int searched_key = key_val.outsidekey;
  
  //A traversal search as usual

  while (curr_node->is_leaf == false) 
  {
    int min = -1;
    int max = curr_node->nb_keys;

    while (max-min >1)
    {
      int cur_idx = (max+min)/2;
      int cur_key = curr_node->key_val[cur_idx].idxkey;

      if(cur_key <= searched_key)
      {
        min = cur_idx;
      }
      else{
        max = cur_idx;
      }
    }
    //At this point, we know the index of the children which has to be the next node to look in
    struct idxnode *correct_child = curr_node->children[max];
    //if the next node is full, we have to split it in two nodes before going down to the correct node

    if (correct_child->nb_keys < ORDER-1)
    {
      curr_node = correct_child;
      continue;
    }

    else 
    {
      curr_node = idxsplitChild(curr_node, max,searched_key);  
    } 
  }
  //We have arrived on the only node that could countain the key we want to insert, we have to check if the keys is already present
  bool already = false;
  bool alreadyinside = false;
  int correct_outside= 0;

  for (int j=0;j<curr_node->nb_keys;j++)
  {
    already = already | curr_node->key_val[j].idxkey == key_val.outsidekey;
    correct_outside = j;
  }
  //if it doesn't already exist, we have to insert it in the correct index

  if (!already)
  {
    struct idxkey_value new_key_val[ORDER-1];
    //we look through all the keys which are already sorted and populate a new array of keys including the one to insert.
    // We don't have to worry about the children array as the leaf nodes don't have children
    int i;

    for (i=0;i< curr_node->nb_keys;i++)
    {

      if (curr_node->key_val[i].idxkey > key_val.outsidekey)
      {
        break;
      }
      new_key_val[i]=curr_node->key_val[i];
    }
    //We found the Outside key
    struct idxdata *data_to_insert;
    data_to_insert = malloc(sizeof(*data_to_insert));
    data_to_insert ->size = 1;
    data_to_insert->keyarray = malloc(sizeof(*data_to_insert->keyarray));
    data_to_insert->keyarray[0] = key_val.insidekey;
    new_key_val[i].value = data_to_insert;
    new_key_val[i].idxkey = key_val.outsidekey;

    for (int j=i+1;j< curr_node->nb_keys+1;j++)
    {
      new_key_val[j] = curr_node -> key_val[j-1];
    }
    //we replace the node's keys array by the new one
    memcpy(&(curr_node->key_val),&new_key_val,sizeof(new_key_val));
    //not forget to increment the node's number of key attribute
    curr_node->nb_keys++;
  }

  else 
  {
    int sz_keys = curr_node->key_val[correct_outside].value->size;

    for (int kix=0;kix<sz_keys;kix++)
    {
      alreadyinside |= curr_node->key_val[correct_outside].value->keyarray[kix] == key_val.insidekey;
    }

    if (!alreadyinside)
    {
      curr_node->key_val[correct_outside].value->keyarray[sz_keys] = key_val.insidekey;
      curr_node->key_val[correct_outside].value->size++;
    }

    else
    {
      printf("Insertion of key failed, the key was already existing\n");
    }
  }

  printf("FIN INSERT\n");
  return 1;
}


//yabdelete performs a traversal search as yabinsert in order to delete a key-value pair
int yaidxbdelete(idxbtree t,struct singlepair sgpair)
{
  int key = sgpair.outsidekey;
  struct idxnode *curr_node = t;

  while (curr_node->is_leaf == false) 
  {
    int min = -1;
    int max = curr_node->nb_keys;

    while (max-min >1)
    {
      int cur_idx = (max+min)/2;
      int cur_key = curr_node->key_val[cur_idx].idxkey;

      if(cur_key < key)
      {
        min = cur_idx;
      }

      else
      {
        max = cur_idx;
      }
    }
    //At this point, we know the index of the children which has to be the next node to look in
    struct idxnode *correct_child = curr_node->children[max];
    curr_node = correct_child;

  }
  //As for yabinsert, we populate the keys-value array surpressing the right one
  struct idxkey_value new_key_val[ORDER-1];
  bool found_and_deleted=false;
  bool foundkey =false;

  for (int i=0;i<curr_node->nb_keys;i++)
  {
    if (curr_node->key_val[i].idxkey !=key)
    {
      new_key_val[i-found_and_deleted]=curr_node->key_val[i];
    }

    else 
    {
      int sz_idx_keys = curr_node-> key_val[i].value->size;
      int *new_idx_keys;
      new_idx_keys = malloc(sizeof(*new_idx_keys));
      int tmp=0;

      for (int j=0;j<sz_idx_keys;j++)
      {

        if (curr_node->key_val[i].value->keyarray[j] != sgpair.insidekey)
        {
	        new_idx_keys[tmp] = curr_node->key_val[i].value->keyarray[j];
	        tmp++;
	      }

        else 
        {
          curr_node->key_val[i].value->size--;
          foundkey = true;
        }
      }
      free(curr_node->key_val[i].value->keyarray);
      curr_node->key_val[i].value->keyarray =new_idx_keys;
      printf ("AAAAAAA\n");

      if (curr_node->key_val[i].value->size != 0){
        printf("ooooo  %d\n", i-found_and_deleted);
        new_key_val[i-found_and_deleted]=curr_node->key_val[i];
      }

      else 
      {
        found_and_deleted=true;
      }
    }
  }

  if (!foundkey)
  {
    printf("Deletion failed, key not found among the keyarray\n");
    return 1;
  }
  
  else 
  {
    printf("mmmmm  %d\n", new_key_val[2].idxkey);
    if (found_and_deleted){
      curr_node->nb_keys--;
    }
    memcpy(&(curr_node->key_val),&new_key_val,sizeof(new_key_val));
  }
  return 1;

}
