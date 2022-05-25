/* Type of the node, either it's internal or a leaf */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*typedef enum {Node_branch, Node_leaf} NodeType;*/


#define ORDER 5

struct data
{
  char *prenom;
  char *nom;
  int age;
};


struct key_value
{
  int key;
  struct data *value;
};




struct node
{
  bool is_leaf;
  int nb_keys;
  struct key_value key_val[ORDER-1];
  struct node *children[ORDER];
};

typedef struct node *btree;



btree
yabcreate(void);

btree
yabinsert(btree tree,struct key_value key_val);

struct data*
yabsearch(btree tree, int key);

btree
yabdelete(btree tree, int key);
void
node_to_string(struct node* nod);
