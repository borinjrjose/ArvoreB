//ArvoreB.h
#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    int numChaves;
    int posDisco;
    bool folha;
    int* chaves;
    int* filhos;
}No;

typedef struct
{
    int proxVazio, fimArquivo;
}PilhaDisco;

typedef struct
{
    int ordem, raizDisco;
    No* raiz;
    PilhaDisco* p;
}ArvoreB;

PilhaDisco* novaPilha();
void push(PilhaDisco* p, int i, int ordem, FILE* fp);
int pop(PilhaDisco* p, int ordem, FILE* fp);

No* alocarNo(int ordem);
void desalocarNo(No* n);

int calcularOffset(int i, int ordem);
void escreverDisco(No* n, int ordem, FILE* fp);
No* lerDisco(int pos, int ordem, FILE* fp);

void atualizarCabecalho(ArvoreB* ar, FILE* fp);
void carregarCabecalho(ArvoreB* ar, FILE* fp);
ArvoreB* novaArvore(int ordem, FILE* fp);
ArvoreB* carregarArvore(FILE* fp);

No* procurar(No* n, int* i, int ordem, int chave, FILE* fp);
void inserir(ArvoreB* ar, int chave, FILE* fp);
void dividir(ArvoreB* ar, No* x, int i, FILE* fp);
void inserirNaoVazio(ArvoreB* ar, No* n, int chave, FILE* fp);
void excluir(ArvoreB* ar, No* n, int chave, FILE* fp);
void juntar(ArvoreB* ar, No* x, int i, FILE* fp);
