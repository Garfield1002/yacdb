#include "nodes.h"

btree yabcreate(void)
{
  btree t;
  t = malloc(sizeof(*t));
  t->is_leaf = true;
  t->nb_keys = 0;
  return t;
}

btree bcreate_start(struct key_value key_v)
{
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

void node_to_string(struct node *nod)
{
  char string[1024];
  printf("[ %d |", nod->nb_keys);
  for (int i = 0; i < nod->nb_keys; i++)
  {
    printf(" %d |", nod->key_val[i].key);
  }
  printf("\n");
}

struct data *yabsearch(btree t, int searched_key)
{
  // We have to find out if the searched_key is among the keys of the current node. If not, we have to find the only 'sub-tree' that can contain the key.
  // As the keys are sorted, we can use dichotomical search instead of classic search

  int min = -1;
  int max = t->nb_keys;

  while (max - min > 1)
  {
    int cur_idx = (max + min) / 2;
    // if the key is found out, retrieve the corresponding value
    int cur_key = t->key_val[cur_idx].key;
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
    else if (cur_key < searched_key)
    {
      min = cur_idx;
    }
    else
    {
      max = cur_idx;
    }
  }
  // if we get to this point, that means we haven't found the target in the current node, we have to search into the correct 'subtree'
  // At this point, min and max are the virtual indexes (because min can be equal to -1 and max equal to nb_keys) of the two keys between which the searched key is.

  if (t->is_leaf)
  {
    return 0;
  }

  int child_idx = max;

  return yabsearch(t->children[child_idx], searched_key);
}

struct node *splitChild(struct node *old_parent, int idx_child_to_split, int searched_key)
{
  printf("SPLITCHILD\n");
  struct node *correct_child = old_parent->children[idx_child_to_split];
  // First, we have to find out the key_value pair that's around the middle of the pairs list of the children and its index.
  int mid_idx = (correct_child->nb_keys) / 2;
  mid_idx--;
  struct key_value pair_to_up = correct_child->key_val[mid_idx];

  // we create the new nodes product by the splitting
  struct node *new_left_node;
  struct node *new_right_node;
  new_left_node = malloc(sizeof(*new_left_node));
  new_right_node = malloc(sizeof(*new_right_node));
  // We initialize the nb of key_val pairs which will be incremented as we fill up the lists
  new_left_node->nb_keys = 0;
  new_right_node->nb_keys = 0;

  // If the old child was a leaf then the new ones are too, otherwise they're not
  new_left_node->is_leaf = correct_child->is_leaf;
  new_right_node->is_leaf = correct_child->is_leaf;

  // We start by filling the left node
  int tmp;
  for (int i = 0; i <= mid_idx; i++)
  {
    new_left_node->nb_keys++;
    new_left_node->key_val[i] = correct_child->key_val[i];
    new_left_node->children[i] = correct_child->children[i];
    if (!(new_left_node->is_leaf))
    {
      printf("ADD on left node; child number %d\n", i);
      node_to_string(new_left_node->children[i]);
    }
    tmp = i;
  }
  // Not forget to add the rightmost child
  new_left_node->children[tmp] = correct_child->children[tmp];
  // As it is a B+ Tree, we have to get back the
  //  new_left_node->key_val[new_left_node->nb_keys]=pair_to_up;
  //  new_left_node->nb_keys++;
  int tmp2;
  // Now, let's fill the right one
  for (int i = mid_idx + 1; i < correct_child->nb_keys; i++)
  {
    new_right_node->nb_keys++;
    printf("La valeur de %d de correct_child vaut %d\n", i, correct_child->key_val[i].key);
    new_right_node->key_val[i - mid_idx - 1] = correct_child->key_val[i];
    new_right_node->children[i - mid_idx - 1] = correct_child->children[i];
    tmp2 = i;
  }
  printf("Left node --------\n");
  node_to_string(new_left_node);
  printf("Right node --------\n");
  node_to_string(new_right_node);
  printf("--------------------\n");

  // Not forget to add the rightmost child
  new_right_node->children[tmp2 - mid_idx - 1] = correct_child->children[tmp2];

  struct key_value old_pairs[ORDER - 1];
  struct node *old_children[ORDER];
  memcpy(&old_pairs, &(old_parent->key_val), sizeof(old_parent->key_val));
  memcpy(&old_children, &(old_parent->children), sizeof(old_parent->children));

  old_parent->key_val[idx_child_to_split] = pair_to_up;
  old_parent->children[idx_child_to_split] = new_left_node;
  old_parent->children[idx_child_to_split + 1] = new_right_node;
  old_parent->nb_keys++;

  for (int new_idx = idx_child_to_split + 1; new_idx < old_parent->nb_keys; new_idx++)
  {
    old_parent->key_val[new_idx] = old_pairs[new_idx - 1];
    old_parent->children[new_idx + 1] = old_children[new_idx];
  }

  printf("children---------\n");
  for (int ch = 0; ch <= old_parent->nb_keys; ch++)
  {

    node_to_string(old_parent->children[ch]);
    if (!(old_parent->children[ch]->is_leaf))
    {
      for (int ch2 = 0; ch2 < old_parent->children[ch]->nb_keys; ch2++)
      {
        printf("child number %d\n", ch2);
        struct node *chil = old_parent->children[ch];
        node_to_string(chil->children[ch2]);
        node_to_string(new_left_node->children[ch2]);
      }
    }
    printf("%d\n", old_parent->children[ch]->nb_keys);
  }
  printf("--------------------\n");

  printf("%d\n", old_parent->nb_keys);
  printf("FIN SPLITCHILD\n");
  printf("La valeur de la clé remontée : %d\n", pair_to_up.key);
  if (pair_to_up.key < searched_key)
  {
    return new_right_node;
  }
  else
  {
    return new_left_node;
  }
}

btree yabinsert(btree t, struct key_value key_val)
{
  printf("Le nb de clés au debut de yabinsert : %d\n", t->nb_keys);
  if (t->nb_keys >= ORDER - 1)
  {
    struct node *virt_racine;
    virt_racine = malloc(sizeof(*virt_racine));
    virt_racine->is_leaf = false;
    virt_racine->nb_keys = 0;
    virt_racine->children[0] = t;
    struct node *useless_result = splitChild(virt_racine, 0, 999);
    node_to_string(useless_result);
    printf("RACINE VIRTUELLE ---------\n");
    node_to_string(virt_racine);
    printf("----------------\n");

    return yabinsert(virt_racine, key_val);
  }
  // we start from the root, we check if we came to a leaf
  node_to_string(t);
  struct node *curr_node = t;
  // The value of the key to insert
  int searched_key = key_val.key;
  while (curr_node->is_leaf == false)
  {
    int min = -1;
    int max = t->nb_keys;

    while (max - min > 1)
    {
      int cur_idx = (max + min) / 2;
      //      printf("idx vaut %d ; max vaut %d ; min vaut %d ; nb_keys vaut %d \n",cur_idx,max,min,t->nb_keys);
      int cur_key = t->key_val[cur_idx].key;
      if (cur_key <= searched_key)
      {
        min = cur_idx;
      }
      else
      {
        max = cur_idx;
      }
    }
    // At this point, we know the index of the children which has to be the next node to look in
    struct node *correct_child = curr_node->children[max];
    printf("On descend pour chercher le bon noeud\n");
    node_to_string(correct_child);
    printf("Le nombre de clés de correct_child vaut %d\n", correct_child->nb_keys);
    if (correct_child->nb_keys < ORDER - 1)
    {
      curr_node = correct_child;
      continue;
    }
    else
    {
      node_to_string(curr_node);
      curr_node = splitChild(curr_node, max, searched_key);

      // TEST
      printf("Tree after splitChild------------\n");
      node_to_string(t);
      for (int i = 0; i < t->nb_keys; i++)
      {
        node_to_string(t->children[i]);
      }
      node_to_string(t->children[t->nb_keys]);
      printf("---------------------------\n");
      node_to_string(curr_node);
    }
  }
  bool already = false;
  for (int j = 0; j < curr_node->nb_keys; j++)
  {
    already = already | curr_node->key_val[j].key == key_val.key;
  }
  printf("aa\n");
  if (!already)
  {
    printf("La clé est ajouté sur \n");
    node_to_string(curr_node);
    struct key_value new_key_val[ORDER - 1];
    int idx = 0;
    for (int i = 0; i < curr_node->nb_keys; i++)
    {

      if (curr_node->key_val[idx].key == key_val.key)
      {
        printf("Insertion failed, key already existing !\n");
        return t;
      }

      if (curr_node->key_val[idx].key > key_val.key)
      {
        break;
      }
      idx++;

      new_key_val[i] = curr_node->key_val[i];
    }
    new_key_val[idx] = key_val;

    for (int i = idx + 1; i < curr_node->nb_keys + 1; i++)
    {
      new_key_val[i] = curr_node->key_val[i - 1];
    }
    memcpy(&(curr_node->key_val), &new_key_val, sizeof(new_key_val));
    //    curr_node->key_val[curr_node->nb_keys]=key_val;
    curr_node->nb_keys++;
  }

  else
  {
    printf("Insertion of key failed, the key was already existing\n");
  }

  printf("Node after insert\n");
  node_to_string(curr_node);
  return t;
}

btree yabdelete(btree t, int key)
{
  struct node *curr_node = t;
  while (curr_node->is_leaf == false)
  {
    int min = -1;
    int max = t->nb_keys;

    while (max - min > 1)
    {
      int cur_idx = (max + min) / 2;
      //      printf("idx vaut %d ; max vaut %d ; min vaut %d ; nb_keys vaut %d \n",cur_idx,max,min,t->nb_keys);
      int cur_key = t->key_val[cur_idx].key;
      if (cur_key < key)
      {
        min = cur_idx;
      }
      else
      {
        max = cur_idx;
      }
    }
    // At this point, we know the index of the children which has to be the next node to look in
    struct node *correct_child = curr_node->children[max];
    printf("On descend pour chercher le bon noeud\n");
    node_to_string(correct_child);
    curr_node = correct_child;
  }
  struct key_value new_key_val[ORDER - 1];
  bool found = false;
  for (int i = 0; i < curr_node->nb_keys; i++)
  {
    if (curr_node->key_val[i].key != key)
    {
      new_key_val[i - found] = curr_node->key_val[i];
    }
    else
    {
      found = true;
    }
  }
  if (!found)
  {
    printf("Deletion failed, key not found ! \n");
    node_to_string(curr_node);
  }
  else
  {
    curr_node->nb_keys--;
    memcpy(&(curr_node->key_val), &new_key_val, sizeof(new_key_val));
  }
  return t;
}
