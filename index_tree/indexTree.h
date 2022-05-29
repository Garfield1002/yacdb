#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ORDER 5

struct idxdata
{
  int *keyarray;
  int size;
};


typedef struct idxdataaray 
{
  struct idxdata **data;
  int size;
} idxdataaray;

struct idxkey_value
{
  int idxkey;
  struct idxdata *value;
};

struct singlepair
{
  int outsidekey;
  int insidekey;
};

struct idxnode
{
  bool is_leaf;
  int nb_keys;
  struct idxkey_value key_val[ORDER - 1];
  struct idxnode *children[ORDER];
};

typedef struct idxnode *idxbtree;


idxbtree
yaidxbcreate(void);

int
yaidxbinsert(idxbtree tree,struct singlepair key_val);

struct idxdata*
yaidxbsearch(idxbtree tree, int key);

void
yaidxbgetall(idxbtree tree, idxdataaray *allfound);

int
yaidxbdelete(idxbtree tree, struct singlepair sgpair);

void
idxnode_to_string(struct idxnode* nod);

