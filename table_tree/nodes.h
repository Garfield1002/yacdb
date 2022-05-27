#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
  struct key_value key_val[ORDER - 1];
  struct node *children[ORDER];
};

typedef struct node *btree;

typedef struct dataarray {
  struct data **data;
  int size;
} dataarray;

btree
yabcreate(void);

int
yabinsert(btree tree,struct key_value key_val);

struct data*
yabsearch(btree tree, int key);

void
yabgetall(btree tree, dataarray *allfound);

int
yabdelete(btree tree, int key);

void
node_to_string(struct node* nod);

void
dump_tree(btree t);
