/* Type of the node, either it's internal or a leaf */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
/*typedef enum {Node_branch, Node_leaf} NodeType;*/


#define ORDER 10

struct data
{
  char *truc;
};


struct key_value
{
  int key;
  struct data value;
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

void
yabinsert(btree tree,struct key_valu key_val);

struct data
yabsearch(btree tree, int key);

void
yabdelete(btree tree, int key);


