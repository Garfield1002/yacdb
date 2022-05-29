#include "../instructions_to_tree.h"

int main()
{
    struct CrtInstr *testcreate;
    testcreate = malloc(sizeof(*testcreate));
    testcreate->columns = malloc(sizeof(*(testcreate->columns)));
    testcreate->columns->arr = malloc(sizeof(*testcreate->columns->arr));
    testcreate->columns->size=2;
    testcreate->columns->arr[0]= "prenom";
    testcreate->columns->arr[1]= "nom";
    printf("AAAAAAA\n");
    dbtree *alltrees = trees_after_creation(testcreate);
    printf("AAAAAAA\n");
    int nb_trees = alltrees->indextrees->size;
    printf("Nombre d'arbres d'index : %d\n", nb_trees);
    node_to_string(alltrees->maintree);
    idxnode_to_string(alltrees->indextrees->idxbtreearray[0].idxbtree);
    idxnode_to_string(alltrees->indextrees->idxbtreearray[1].idxbtree);

    struct AddInstr *testadd;
    testadd = malloc(sizeof(*testadd));
    testadd->columns = malloc(sizeof(*testadd->columns));
    testadd->values = malloc(sizeof(*testadd->values));
    testadd->columns->arr = malloc(sizeof(*testadd->columns->arr));
    testadd->values->arr = malloc(sizeof(*testadd->values->arr));
    testadd->columns->size = 3;
    testadd->values->size = 3;
    testadd->columns->arr[0]= "id";
    testadd->columns->arr[1]= "nom";
    testadd->columns->arr[2]= "prenom";

    testadd->values->arr[0]="1";
    testadd->values->arr[1]="69";
    testadd->values->arr[2]="666";
    printf("oooo\n");
    itinsert(alltrees, testadd);
    printf("ok");
    
    testadd->values->arr[0]="4";
    testadd->values->arr[1]="63";
    testadd->values->arr[2]="666";
    itinsert(alltrees, testadd);
    node_to_string(alltrees->maintree);
    idxnode_to_string(alltrees->indextrees->idxbtreearray[0].idxbtree);
    idxnode_to_string(alltrees->indextrees->idxbtreearray[1].idxbtree);
}
