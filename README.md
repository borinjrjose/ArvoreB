# Árvore B

Programa que implementa uma Árvore B na linguagem de programação C junto com algumas funções auxiliares com as seguintes finalidades:
* Manipulação de uma pilha em disco que controla espaços livres
* Leitura e escrita de nós no disco
* Alocação e desalocação de nós na memória principal
* Inicialização e manutenção da árvore

## Descrição

Para a manipulação das chaves da ávore, são utilizadas as funções procurar, inserir e excluir juntamente com a função novaArvore para carregar ou criar uma Árvore B a partir de um arquivo. Este arquivo, por sua vez, será controlado pela estrutura de dados em forma de pilha que levará em conta a ordem da árvore para calcular e gerenciar espaços vazios.

## Utilização

O arquivo [main.c](https://github.com/borinjrjose/ArvoreB/blob/main/main.c) possui alguns exemplos de como utilizar a árvore, criando uma nova a partir de um arquivo chamado "ArvoreB.txt", inserindo algumas chaves, excluindo outras e, por fim, procurando uma chave existente e imprimindo as chaves do nó junto com seu tamanho.
