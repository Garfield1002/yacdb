#include "../nodes.h"


int main(){
  struct data tdata1 = {"John","Doe",69};
  struct data tdata2 = {"Jahn","Doe",34};
  struct data tdata3 = {"Jihn","Doe",33};
  struct data tdata4 = {"Juhn","Doe",55};
  struct data tdata5 = {"Jehn","Doe",82};
  struct data tdata6 = {"Jane","Doe",99};
  struct data tdata7 = {"Joe","Doe",12};
  struct data tdata8 = {"Jhin", "Doe", 21};
  struct data tdata9 = {"Jim", "Doe", 61};
  struct data tdata10 = {"Jun", "Doe", 41};
  struct data tdata11 = {"Jan", "Doe", 51};
  struct data tdata12 = {"Jan", "Doe", 21};
  struct data tdata13 = {"Jimmy", "Doe", 71};
  struct data tdata14 = {"James", "Doe", 111};

  btree ttest = yabcreate();
  struct key_value k1 = {1,&tdata1};
  struct key_value k2 = {2,&tdata2};
  struct key_value k3 = {3,&tdata3};
  struct key_value k4 = {4,&tdata4};
  struct key_value k5 = {5,&tdata5};
  struct key_value k6 = {6,&tdata6};
  struct key_value k7 = {7,&tdata7};
  struct key_value k8 = {8,&tdata8};
  struct key_value k9 = {9,&tdata9};
  struct key_value k10 = {10,&tdata10};
  struct key_value k11 = {44,&tdata11};
  struct key_value k12 = {23,&tdata12};
  struct key_value k13 = {666,&tdata13};
  struct key_value k14 = {11,&tdata14};



  yabinsert(ttest, k1);
  yabinsert(ttest, k2);
  yabinsert(ttest, k3);
  yabinsert(ttest, k4);
  yabinsert(ttest, k5);
  yabinsert(ttest, k6);
  yabinsert(ttest, k7);
  yabinsert(ttest, k8);
  yabinsert(ttest, k9);
  yabinsert(ttest, k10);
  yabinsert(ttest, k11);
  yabinsert(ttest, k12);
  yabinsert(ttest, k13);
  yabinsert(ttest, k14);

  struct data *result;
  result = malloc(sizeof(*result));
  result = yabsearch(ttest,6);

  if (result !=0)
  {
    printf("Resultat obtenu\n");
  }

  result = yabsearch(ttest,65);
 
  if (result !=0){
    printf("Resultat Obtenu\n");
  }

  yabdelete(ttest,2);
  
  node_to_string(ttest);

  for (int i=0; i<ttest->nb_keys+1;i++)
  {
    printf("LEVEL 2---------\n");
    node_to_string(ttest->children[i]);
    printf("LEVEL 3------------\n");

    for (int i2=0;i2<=ttest->children[i]->nb_keys;i2++)
    {
      node_to_string(ttest->children[i]->children[i2]);
    }
    printf("-------------------\n");
  }
  dataarray *all;
  all = malloc(sizeof(*all));
  all->size =0;
  all->data = malloc(sizeof(*(all->data)));
  printf("%d\n", all->size);

  yabgetall(ttest,all);
  printf("%d\n", all->size);

  dump_tree(ttest);
  
}
