#include <stdio.h>
#include <stdlib.h>
#include "ArvoreB.h"

int main()
{
    FILE* fp = fopen("ArvoreB.txt", "w+");
    ArvoreB* ar = novaArvore(2, fp);
    inserir(ar, 1, fp);
    inserir(ar, 2, fp);
    inserir(ar, 3, fp);
    inserir(ar, 4, fp);
    //inserir(ar, 5, fp);
    //inserir(ar, 6, fp);
    //inserir(ar, 7, fp);

    excluir(ar, ar->raiz, 4, fp);
    excluir(ar, ar->raiz, 3, fp);

    int i;

    No* n = procurar(ar->raiz, &i, ar->ordem, 1, fp);
    printf("%i ", n->numChaves);


    for(int j = 0; j < n->numChaves; j++)
        printf("%i ", n->chaves[j]);
    printf("\n");

    return 0;
}
