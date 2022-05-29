#include "instructions_to_tree.h"
#include "../server-side/parser/include/instruction.h"
/*
char **
int_charer(long long nb)
{
  char val[10];
  val[0]= (nb >> 56) & 0xff;
  val[1]= (nb >> 48) & 0xff;
  val[2]= (nb >> 40) & 0xff;
  val[3]= (nb >> 32) & 0xff;
  val[4]= (nb >> 24) & 0xff;
  val[5]= (nb >> 16) & 0xff;
  val[6]= (nb >> 8) & 0xff;
  val[7]= (nb >> 0) & 0xff;
  return &val;
}

long long
char_inter(char *str)
{
  long long resul= 0;

  for (int i=0; str[i]!=0;i++)
  {

  }
}
*/

void translator(dbtree *fulltrees, instr *instruction)
{
  switch (instruction->type) 
  {
    case sel:
    {
      struct SelInstr *selectInstr = (struct SelInstr*) instruction;
      struct dataarray *result = itselect(fulltrees, selectInstr);
      break;
    }
    case crt:
    {
      struct CrtInstr *selectInstr = (struct CrtInstr*) instruction;
    }

    default:
    {
      printf("Instruction not found\n");
    }
  }
}

dbtree *
trees_after_creation(struct CrtInstr *crtInstr)
{
  indexbtreearray *allidxtree;
  allidxtree = malloc(sizeof(*allidxtree));
  allidxtree->size=0;
  allidxtree->idxbtreearray = malloc(sizeof(*allidxtree->idxbtreearray));
  int nb_col = crtInstr->columns->size;

  for (int colidx=0;colidx<nb_col;colidx++)
  {
      idxbtree temptree = yaidxbcreate();
      allidxtree->idxbtreearray[colidx].idxname = crtInstr->columns->arr[colidx];
      allidxtree->idxbtreearray[colidx].idxbtree = temptree;
      allidxtree->size++;
      printf("Arbre inseré\n");
  }
  btree maintree;
  maintree = malloc(sizeof(*maintree));
  maintree->nb_keys=0;
  maintree->is_leaf=true;
  dbtree *fulltrees;
  fulltrees = malloc(sizeof(*fulltrees));
//  fulltrees->indextrees = malloc(sizeof(*(fulltrees->indextrees)));
  fulltrees->indextrees = allidxtree;
  fulltrees->maintree = maintree;
  return fulltrees;
}


dataarray *itselect(dbtree *fulltrees, struct SelInstr *selInstr)
{
  indexbtreearray *idxtrees = fulltrees->indextrees;
  int nb_col_to_select = selInstr->columns->size;
  dataarray *full_result;

  if (!selInstr->has_cond)
  {
    full_result = malloc(sizeof(*full_result));
    full_result->size = 0;
    full_result -> data = malloc(sizeof(*(full_result->data)));
    yabgetall(fulltrees->maintree, full_result);

    if (full_result->size!=0 & nb_col_to_select !=0)
    {
      int nb_col = full_result->data[0]->columns->size;
      int nb_res = full_result->size;

      for (int idxresult=0; idxresult<nb_res;idxresult++)
      {
        struct data *data_without_useless_col;
        data_without_useless_col = malloc(sizeof(*data_without_useless_col));
        data_without_useless_col->columns = malloc(sizeof(data_without_useless_col->columns));
        data_without_useless_col->columns->arr = malloc(sizeof(data_without_useless_col->columns->arr));
        data_without_useless_col->values = malloc(sizeof(data_without_useless_col->values));
        data_without_useless_col->values->arr = malloc(sizeof(data_without_useless_col->values->arr));
        data_without_useless_col->columns->size=0;

        for (int idxcol=0;idxcol<nb_col;idxcol++)
        {
          char * name_col = full_result->data[idxresult]->columns->arr[idxcol];

          for (int idxcolsel=0;idxcolsel <nb_col_to_select;idxcolsel++)
          {

            if (strcmp(name_col,selInstr->columns->arr[idxcolsel])==0)
            {
              int currsz = data_without_useless_col->columns->size;
              data_without_useless_col->columns->arr[currsz] =name_col;
              data_without_useless_col->values->arr[currsz] = full_result->data[idxresult]->values->arr[idxcol];
              data_without_useless_col->columns->size++;
              data_without_useless_col->values->size++;
            }
          }
        }
        full_result->data[idxresult] = data_without_useless_col;
      }
    }
    return full_result;
  }
}


void itinsert(dbtree *fulltrees, struct AddInstr *addInstr)
{
  indexbtreearray *idxtrees = fulltrees->indextrees;
  int nb_insert = addInstr->columns->size;
  int nb_trees = idxtrees->size;
  int primkey;
  //We get the primary key, we don't do anything else as the primary key can be specified at at position different than the first one

  for (int colidx=0;colidx<nb_insert;colidx++)
  {
    char *name_col=addInstr->columns->arr[colidx];
    printf("name %s\n", name_col);

    if (strcmp("id", name_col)==0)
    {
      primkey=atoi(addInstr->values->arr[colidx]);
      printf("HEHE%d\n", primkey);
    }
  }
  struct data* data_to_insert;
  data_to_insert = malloc(sizeof(*data_to_insert));
  data_to_insert->values = malloc(sizeof(*data_to_insert->values));
  data_to_insert->columns = malloc(sizeof(data_to_insert->columns));
  charray *columns_to_insert = malloc(sizeof(*columns_to_insert));
  charray *values_to_insert = malloc(sizeof(values_to_insert));
  columns_to_insert->size = 0;
  values_to_insert->size = 0;

  columns_to_insert->arr = malloc(sizeof(columns_to_insert->arr));
  values_to_insert->arr = malloc(sizeof(values_to_insert->arr));

  for (int colidx=0;colidx<nb_insert;colidx++)
  {
    char *name_col=addInstr->columns->arr[colidx];

    if (name_col !="id")
    {
      int sz = columns_to_insert->size;
      columns_to_insert->arr[sz] = name_col;
      columns_to_insert->size++;
      values_to_insert->size++;
      values_to_insert->arr[sz] = addInstr->values->arr[colidx];

      for (int treeidx=0;treeidx <nb_trees;treeidx++)
      {
        
        if (idxtrees->idxbtreearray[treeidx].idxname == name_col)
        {
          int value_to_insert = atoi(addInstr->values->arr[colidx]);
          struct singlepair *sg;
          sg = malloc(sizeof(*sg));
          sg->outsidekey =value_to_insert;
          sg->insidekey = primkey;
          yaidxbinsert(idxtrees->idxbtreearray[treeidx].idxbtree,*sg);
        }
      }
   }
  }
  data_to_insert->columns = columns_to_insert;
  data_to_insert->values = values_to_insert;
  struct key_value key_val;
  key_val.value = malloc(sizeof(key_val.value));
  key_val.value = data_to_insert;
  key_val.key = primkey;
  printf("ok\n");
  yabinsert(fulltrees->maintree, key_val);
  printf("ok");


}
