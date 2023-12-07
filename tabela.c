#include "tabela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tabela *newTabela() {
  struct tabela *tabela = malloc(sizeof(struct tabela));
  if (tabela == NULL)
    return NULL;
  tabela->head = NULL;
  tabela->tamanho = 0;
  return tabela;
}

void delTabela(struct tabela *tabela) {
  while (tabela->head != NULL) {
    struct no *aux = tabela->head;
    tabela->head = tabela->head->prox;
    free(aux->id);
    free(aux->tipo);
    free(aux);
  }
  free(tabela);
}

struct no *newNode(char *id, int end) {
  struct no *node = malloc(sizeof(struct no));
  node->id = strdup(id);
  node->end = end;
  node->prox = NULL;
  node->tipo = strdup("inteiro");
  return node;
}

int addNode(struct tabela *tabela, char *id) {
  if (busca_tabela_simbolos(tabela, id) == -1) {
    struct no *node = newNode(id, tabela->tamanho);
    tabela->tamanho = tabela->tamanho + 1;
    struct no *aux = tabela->head;
    tabela->head = node;
    node->prox = aux;
    return 1;
  }
  else {
    return -1; 
  }
}

int busca_tabela_simbolos(struct tabela *tabela, char *id) {
  struct no *aux = tabela->head;
  while (aux != NULL) {
    if (strcmp(aux->id, id) == 0)
      return aux->end;
    aux = aux->prox;
  }
  return -1; // caso id não exista na tabela
}