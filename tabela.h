#ifndef TABELA_H
#define TABELA_H

struct no {
  int end;
  char *tipo;
  char *id;
  struct no *prox;
};

struct tabela {
  int tamanho;
  struct no *head;
};

struct tabela *newTabela();
void delTabela(struct tabela *tabela);
struct no *newNode(char *id, int end);
int addNode(struct tabela *tabela, char *id);
int busca_tabela_simbolos(struct tabela *tabela, char *id);

#endif