#include "../nodes.h"


int main(){
  struct data tdata2 = {"John","Doe",69};
  struct data tdata3 = {"Jahn","Doe",34};
  struct data tdata4 = {"Jihn","Doe",33};
  struct data tdata5 = {"Juhn","Doe",55};
  struct data tdata6 = {"Jehn","Doe",82};
  struct data tdata7 = {"Jane","Doe",99};
  struct data tdata8 = {"Joe","Doe",12};

  btree ttest = yabcreate();
  struct key_value k1 = {1,&tdata2};
  struct key_value k2 = {2,&tdata2};
  struct key_value k3 = {3,&tdata2};
  struct key_value k4 = {4,&tdata2};
  struct key_value k5 = {5,&tdata2};
  struct key_value k6 = {6,&tdata2};
  struct key_value k7 = {7,&tdata2};
  struct key_value k8 = {8,&tdata2};




  ttest = yabinsert(ttest, k1);
  ttest = yabinsert(ttest, k2);
  ttest = yabinsert(ttest, k3);
  ttest = yabinsert(ttest, k4);
  ttest = yabinsert(ttest, k5);
  ttest = yabinsert(ttest, k6);
  ttest = yabinsert(ttest, k7);
  ttest = yabinsert(ttest, k8);
  ttest = yabinsert(ttest, k8);
  struct data *result;
  result = malloc(sizeof(*result));
  result = yabsearch(ttest,1);
  printf("Nom : %s, Prenom : %s, Age :%d\n",result->nom,result->prenom,result->age);

  ttest = yabdelete(ttest,4);


  node_to_string(ttest);
}
