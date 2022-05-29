#include "nodes.h"
#include "../server-side/parser/include/instruction.h"
#include "../index_tree/indexTree.h"


typedef struct indexbtree
{
  idxbtree idxbtree;
  char *idxname;
}indexbtree;


typedef struct indexbtreearray
{
 struct indexbtree *idxbtreearray;
 int size;
} indexbtreearray;


typedef struct dbtree
{
    indexbtreearray* indextrees;
    btree maintree;
} dbtree;


void
translator(dbtree *fulltrees, instr *instruction);


dbtree *
trees_after_creation(struct CrtInstr *crtInstr);


dataarray *
itselect(dbtree *fulltrees, struct SelInstr *selectInstr);


void
itinsert(dbtree *fulltrees, struct AddInstr *AddInstr);


long long
char_inter(char *str);


char *
int_charer(long long nb);




