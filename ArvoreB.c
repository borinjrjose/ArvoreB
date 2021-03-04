//ArvoreB.c
#include "ArvoreB.h"
#include <stdlib.h>
#include <stdio.h>

//Funções da Pilha para manipular espaços vazios no disco
PilhaDisco* novaPilha()
{
    PilhaDisco* p = (PilhaDisco*) malloc(sizeof(PilhaDisco));
    p->fimArquivo = 0;
    p->proxVazio = -1;

    return p;
}

void push(PilhaDisco* p, int i, int ordem, FILE* fp)
{
    fseek(fp, calcularOffset(i, ordem), SEEK_SET);
    fwrite(&(p->proxVazio), sizeof(int), 1, fp);
    p->proxVazio = i;
}

int pop(PilhaDisco* p, int ordem, FILE* fp)
{
    int resposta;

    if(p->proxVazio == -1)
    {
        resposta = p->fimArquivo;
        p->fimArquivo = p->fimArquivo + 1;
    }
    else
    {
        resposta = p->proxVazio;
        fseek(fp, calcularOffset(resposta, ordem), SEEK_SET);
        fread(&(p->proxVazio), sizeof(int), 1, fp);
    }

    return resposta;
}

//Funções para manipular Alocação de Nós
No* alocarNo(int ordem)
{
    No* resposta = (No*) malloc(sizeof(No));
    resposta->chaves = (int*) malloc(sizeof(int)*(2*ordem-1));
    resposta->filhos = (int*) malloc(sizeof(int)*(2*ordem));

    return resposta;
}

void desalocarNo(No* n)
{
    free(n->chaves);
    free(n->filhos);
    free(n);
}

//Funções para Leitura e Escrita no Disco dos Nós
int calcularOffset(int i, int ordem)
{
    int tamCabecalho = sizeof(int)*4;
    int tamNo = sizeof(int)*2 + sizeof(bool) + sizeof(int)*(2*ordem-1) + sizeof(int)*(2*ordem);
    return tamCabecalho + i*tamNo;
}

void escreverDisco(No* n, int ordem, FILE* fp)
{
    fseek(fp, calcularOffset(n->posDisco, ordem), SEEK_SET);
    fwrite(&(n->numChaves), sizeof(int), 1, fp);
    fwrite(&(n->posDisco), sizeof(int), 1, fp);
    fwrite(&(n->folha), sizeof(bool), 1, fp);
    fwrite(n->chaves, sizeof(int), n->numChaves, fp);
    if(!n->folha)
        fwrite(n->filhos, sizeof(int), n->numChaves+1, fp);
}

No* lerDisco(int pos, int ordem, FILE* fp)
{
    No* resposta = alocarNo(ordem);

    fseek(fp, calcularOffset(pos, ordem), SEEK_SET);
    fread(&(resposta->numChaves), sizeof(int), 1, fp);
    fread(&(resposta->posDisco), sizeof(int), 1, fp);
    fread(&(resposta->folha), sizeof(bool), 1, fp);
    fread(resposta->chaves, sizeof(int), resposta->numChaves, fp);
    if(!resposta->folha)
        fread(resposta->filhos, sizeof(int), resposta->numChaves+1, fp);

    return resposta;
}

//Funções para Inicialização e Manutenção da Árvore B
void atualizarCabecalho(ArvoreB* ar, FILE* fp)
{
    fseek(fp, 0, SEEK_SET);
    fwrite(&(ar->ordem), sizeof(int), 1, fp);
    fwrite(&(ar->raizDisco), sizeof(int), 1, fp);
    fwrite(&(ar->p->proxVazio), sizeof(int), 1, fp);
    fwrite(&(ar->p->fimArquivo), sizeof(int), 1, fp);
}

void carregarCabecalho(ArvoreB* ar, FILE* fp)
{
    fseek(fp, 0, SEEK_SET);
    fread(&(ar->ordem), sizeof(int), 1, fp);
    fread(&(ar->raizDisco), sizeof(int), 1, fp);
    fread(&(ar->p->proxVazio), sizeof(int), 1, fp);
    fread(&(ar->p->fimArquivo), sizeof(int), 1, fp);
}

ArvoreB* novaArvore(int ordem, FILE* fp)
{
    ArvoreB* resposta = (ArvoreB*) malloc(sizeof(ArvoreB));
    resposta->p = novaPilha();
    resposta->raiz = alocarNo(ordem);
    resposta->ordem = ordem;
    resposta->raizDisco = pop(resposta->p, resposta->ordem, fp);

    resposta->raiz->folha = true;
    resposta->raiz->numChaves = 0;
    resposta->raiz->posDisco = resposta->raizDisco;

    atualizarCabecalho(resposta, fp);
    escreverDisco(resposta->raiz, resposta->ordem, fp);

    return resposta;
}

ArvoreB* carregarArvore(FILE* fp)
{
    ArvoreB* resposta = (ArvoreB*) malloc(sizeof(ArvoreB));
    resposta->p = novaPilha();
    carregarCabecalho(resposta, fp);
    resposta->raiz = lerDisco(resposta->raizDisco, resposta->ordem, fp);

    return resposta;;
}

//Funções da Árvore B
No* procurar(No* n, int* i, int ordem, int chave, FILE* fp)
{
    *i = 0;
    while(n->chaves[*i] < chave && *i < n->numChaves)
        *i = *i + 1;

    if(*i < n->numChaves && n->chaves[*i] == chave)
        return n;

    if(n->folha)
        return NULL;

    No* m = lerDisco(n->filhos[*i], ordem, fp);
    No* resp = procurar(m, i, ordem, chave, fp);

    if(m != resp)
        desalocarNo(m);

    return resp;
}

void inserir(ArvoreB* ar, int chave, FILE* fp)
{
    int k;
    No* atual;
    if((atual = procurar(ar->raiz, &k, ar->ordem, chave, fp)))
    {
        if(atual != ar->raiz) free(atual);
        return;
    }

    if(ar->raiz->numChaves == 2*ar->ordem-1)
    {
        No* s = alocarNo(ar->ordem);
        s->folha = false;
        s->numChaves = 0;
        s->filhos[0] = ar->raiz->posDisco;
        s->posDisco = pop(ar->p, ar->ordem, fp);

        desalocarNo(ar->raiz);

        ar->raiz = s;
        ar->raizDisco = s->posDisco;

        atualizarCabecalho(ar, fp);

        dividir(ar, s, 0, fp);
        inserirNaoVazio(ar, s, chave, fp);
    }
    else
        inserirNaoVazio(ar, ar->raiz, chave, fp);
}

void dividir(ArvoreB* ar, No* x, int i, FILE* fp)
{
    No* y = lerDisco(x->filhos[i], ar->ordem, fp);

    No* z = alocarNo(ar->ordem);
    z->folha = y->folha;
    z->posDisco = pop(ar->p, ar->ordem, fp);

    int j;
    for(j=0; j < ar->ordem-1; j++)
        z->chaves[j] = y->chaves[j+ar->ordem];
    if(!y->folha)
        for(j=0; j < ar->ordem; j++)
            z->filhos[j] = y->filhos[j+ar->ordem];

    z->numChaves = ar->ordem-1;

    for(j=x->numChaves-1; j >= i; j--)
        x->chaves[j+1] = x->chaves[j];

    for(j=x->numChaves; j > i; j--)
        x->filhos[j+1] = x->chaves[j];

    x->chaves[i] = y->chaves[ar->ordem-1];
    x->filhos[i+1] = z->posDisco;
    x->numChaves = x->numChaves+1;

    y->numChaves = ar->ordem-1;

    atualizarCabecalho(ar, fp);
    escreverDisco(x, ar->ordem, fp);
    escreverDisco(y, ar->ordem, fp);
    escreverDisco(z, ar->ordem, fp);

    desalocarNo(y);
    desalocarNo(z);
}

void inserirNaoVazio(ArvoreB* ar, No* n, int chave, FILE* fp)
{
    if(n->folha)
    {
        int i = n->numChaves-1;
        while(i >= 0 && chave < n->chaves[i])
        {
            n->chaves[i+1] = n->chaves[i];
            i--;
        }

        n->chaves[i+1] = chave;
        n->numChaves = n->numChaves+1;

        escreverDisco(n, ar->ordem, fp);
    }
    else
    {
        int i=0;
        while(i < n->numChaves && n->chaves[i] < chave)
            i++;

        No* m = lerDisco(n->filhos[i], ar->ordem, fp);

        if(m->numChaves == 2*ar->ordem-1)
        {
            desalocarNo(m);
            dividir(ar, n, i, fp);

            if(n->chaves[i] < chave)
                m = lerDisco(n->filhos[i+1], ar->ordem, fp);
            else
                m = lerDisco(n->filhos[i], ar->ordem, fp);
        }

        inserirNaoVazio(ar, m, chave, fp);
        desalocarNo(m);
    }
}

void excluir(ArvoreB* ar, No* n, int chave, FILE* fp)
{
    if(n->folha)
    {
        int i=0;
        while(i < n->numChaves && n->chaves[i] < chave)
            i++;

        if(i < n->numChaves && n->chaves[i] == chave)
        {
            for(; i < n->numChaves-1; i++)
                n->chaves[i] = n->chaves[i+1];
            n->numChaves = n->numChaves-1;
            escreverDisco(n, ar->ordem, fp);
        }
    }
    else
    {
        int i=0;
        while(i < n->numChaves && n->chaves[i] < chave)
            i++;

        if(i < n->numChaves && n->chaves[i] == chave)
        {
            No* m = lerDisco(n->filhos[i], ar->ordem, fp);

            if(m->numChaves > ar->ordem-1)
            {
                n->chaves[i] = m->chaves[m->numChaves-1];
                escreverDisco(n, ar->ordem, fp);
                excluir(ar, m, m->chaves[m->numChaves-1], fp);
                desalocarNo(m);
            }
            else
            {
                desalocarNo(m);
                No* o = lerDisco(n->filhos[i+1], ar->ordem, fp);

                if(o->numChaves > ar->ordem-1)
                {
                    n->chaves[i] = o->chaves[0];
                    escreverDisco(n, ar->ordem, fp);
                    excluir(ar, o, o->chaves[0], fp);
                    desalocarNo(o);
                }
                else
                {
                    desalocarNo(o);
                    juntar(ar, n, i, fp);
                    m = lerDisco(n->filhos[i], ar->ordem, fp);

                    if(n == ar->raiz && n->numChaves == 0)
                    {
                        ar->raiz = m;
                        ar->raizDisco = m->posDisco;
                        push(ar->p, n->posDisco, ar->ordem, fp);
                        atualizarCabecalho(ar, fp);
                        desalocarNo(n);
                    }

                    excluir(ar, m, chave, fp);

                    if(m != ar->raiz)
                        desalocarNo(m);
                }
            }
        }
        else
        {
            No* filho = lerDisco(n->filhos[i], ar->ordem, fp);

            if(filho->numChaves == ar->ordem-1)
            {
                if(i-1 >= 0)
                {
                    No* esqFilho = lerDisco(n->filhos[i-1], ar->ordem, fp);

                    if(esqFilho->numChaves > ar->ordem-1)
                    {
                        int j;
                        for(j=filho->numChaves-2; 0 <= j; j--)
                            filho->chaves[j+1] = filho->chaves[j];
                        filho->chaves[0] = n->chaves[i-1];

                        if(!filho->folha)
                        {
                            for(j=filho->numChaves-1; 0 <= j; j--)
                                filho->filhos[j+1] = filho->filhos[j];
                            filho->filhos[0] = esqFilho->filhos[esqFilho->numChaves];
                        }

                        filho->numChaves = filho->numChaves+1;

                        n->chaves[i-1] = esqFilho->chaves[esqFilho->numChaves-1];

                        esqFilho->numChaves = esqFilho->numChaves-1;

                        escreverDisco(esqFilho, ar->ordem, fp);
                        escreverDisco(n, ar->ordem, fp);
                        escreverDisco(filho, ar->ordem, fp);

                        desalocarNo(esqFilho);
                        excluir(ar, filho, chave, fp);
                        desalocarNo(filho);
                        return;
                    }

                    desalocarNo(esqFilho);
                }

                if(i+1 <= n->numChaves)
                {
                    No* dirFilho = lerDisco(n->filhos[i+1], ar->ordem, fp);

                    if(dirFilho->numChaves > ar->ordem-1)
                    {
                        filho->chaves[filho->numChaves] = n->chaves[i];

                        if(!filho->folha)
                            filho->filhos[filho->numChaves+1] = dirFilho->filhos[0];

                        filho->numChaves = filho->numChaves+1;

                        n->chaves[i] = dirFilho->chaves[0];

                        int j;
                        for(j=0; j < dirFilho->numChaves-1; j++)
                            dirFilho->chaves[j] = dirFilho->chaves[j+1];

                        if(!dirFilho->folha)
                            for(j=0; j < dirFilho->numChaves; j++)
                                dirFilho->filhos[j] = dirFilho->filhos[j+1];

                        dirFilho->numChaves = dirFilho->numChaves-1;

                        escreverDisco(filho, ar->ordem, fp);
                        escreverDisco(n, ar->ordem, fp);
                        escreverDisco(dirFilho, ar->ordem, fp);

                        desalocarNo(dirFilho);
                        excluir(ar, filho, chave, fp);
                        desalocarNo(filho);
                        return;
                    }

                    desalocarNo(dirFilho);
                }

                desalocarNo(filho);

                if(i-1 >= 0)
                {
                    juntar(ar, n, i-1, fp);
                    filho = lerDisco(n->filhos[i-1], ar->ordem, fp);
                }
                else
                {
                    juntar(ar, n, i, fp);
                    filho = lerDisco(n->filhos[i], ar->ordem, fp);
                }

                if(n == ar->raiz && n->numChaves == 0)
                {
                    ar->raiz = filho;
                    ar->raizDisco = filho->posDisco;
                    push(ar->p, n->posDisco, ar->ordem, fp);
                    atualizarCabecalho(ar, fp);
                    desalocarNo(n);
                }

                excluir(ar, filho, chave, fp);

                if(filho != ar->raiz)
                    desalocarNo(filho);
            }
            else
            {
                excluir(ar, filho, chave, fp);
                desalocarNo(filho);
            }
        }
    }
}

void juntar(ArvoreB* ar, No* x, int i, FILE* fp)
{
    No* y = lerDisco(x->filhos[i], ar->ordem, fp);
    No* z = lerDisco(x->filhos[i+1], ar->ordem, fp);

    y->chaves[y->numChaves] = x->chaves[i];

    int j;
    for(j=0; j < z->numChaves; j++)
        y->chaves[j+y->numChaves+1] = z->chaves[j];

    if(!y->folha)
        for(j=0; j <= z->numChaves; j++)
            y->filhos[j+y->numChaves+1] = z->filhos[j];

    y->numChaves = y->numChaves + z->numChaves + 1;

    for(j=i; j < x->numChaves-1; j++)
        x->chaves[j] = x->chaves[j+1];

    for(j=i+1; j < x->numChaves; j++)
        x->filhos[j] = x->filhos[j+1];

    x->numChaves = x->numChaves-1;

    push(ar->p, z->posDisco, ar->ordem, fp);

    atualizarCabecalho(ar, fp);
    escreverDisco(x, ar->ordem, fp);
    escreverDisco(y, ar->ordem, fp);

    desalocarNo(y);
    desalocarNo(z);
}
