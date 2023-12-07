#include "tabela.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Lucas Paulo da Rocha - 32196628
// Luiz Octavio Tassinari Saraiva - 30230411

// ########### TIPOS DOS ANALISADOR LEXICO E SINTATICO

typedef enum {
  VARIAVEL,
  DOIS_PONTOS,
  ERRO,
  IDENTIFICADOR,
  NUMERO,
  OP_SOMA,
  OP_MULT,
  PONTO_VIRGULA,
  PONTO,
  ALGORITMO,
  EOS,
  VIRGULA,
  INTEIRO,
  LOGICO,
  COMENTARIO,
  INICIO,
  FIM,
  SE,
  ENQUANTO,
  ABRE_PARENTESES,
  FECHA_PARENTESES,
  ENTAO,
  SENAO,
  LEIA,
  ESCREVA,
  IGUAL,
  FACA,
  MENOR,
  MENOR_IGUAL,
  HASHTAG,
  MAIOR,
  MAIOR_IGUAL,
  OP_SUBTRACAO,
  OU,
  DIV,
  E,
  VERDADEIRO,
  FALSO,
  ATRIBUICAO
} TAtomo;

typedef struct {
  TAtomo atomo;
  int linha;
  float atributo_numero;
  char atributo_ID[100];
} TInfoAtomo;

// ########################################################
// variavel global e funcoes DECLARADA NO ANALISADOR LEXICO
// ########################################################

char *buffer;
int contaLinha = 1;
int proximo_rotulo_contador = 1;

TInfoAtomo obter_atomo();
TInfoAtomo reconhece_numero();
TInfoAtomo reconhece_id();

// ###################################################
// variaveis globais e funcoes DO ANALISADOR SINTATICO
// ###################################################

char strAtomo[][30] = {"VARIAVEL",
                       "DOIS PONTOS",
                       "ERRO LEXICO",
                       "IDENTIFICADOR",
                       "NUMERO",
                       "OP DE SOMA",
                       "OP MULTIPLICACAO",
                       "PONTO E VIRGULA",
                       "PONTO",
                       "ALGORTIMO",
                       "FIM ANALISE SINTATICA",
                       "VIRGULA",
                       "INTEIRO",
                       "LOGICO",
                       "COMENTARIO",
                       "INICIO",
                       "FIM",
                       "SE",
                       "ENQUANTO",
                       "ABRE PARENTESES",
                       "FECHA PARENTESES",
                       "ENTAO",
                       "SENAO",
                       "LEIA",
                       "ESCREVA",
                       "IGUAL",
                       "FACA",
                       "MENOR",
                       "MENOR_IGUAL",
                       "HASHTAG",
                       "MAIOR",
                       "MAIOR IGUAL",
                       "OP SUBTRACAO",
                       "OU",
                       "DIV",
                       "E",
                       "VERDADEIRO",
                       "FALSO",
                       "ATRIBUICAO"};

TInfoAtomo InfoAtomo;
TAtomo lookahead; // lookahead = obter_atomo()

void programa(); // prototipacao de funcao
void consome(TAtomo atomo);
void bloco();
void declaracao_var();
void comando_composto();
void lista_var(int flag);
void tipo();
void comando();
void comando_se();
void comando_atribuicao();
void comando_saida();
void comando_entrada();
void comando_enquanto();
void expressao();
void relacional();
void expressao_simples();
void termo(int *key);
void fator();
int busca();
void adicionaNo();
void erroSemantico();

void leitura(char *arq) {
  FILE *fpointer = fopen(arq, "r");
  if (fpointer == NULL)
    printf("Erro no arquivo");
  fseek(fpointer, 0, SEEK_END);
  long size = ftell(fpointer);
  rewind(fpointer);
  char *inputs = (char *)calloc(size + 1, sizeof(char));
  fread(inputs, 1, size, fpointer);
  buffer = inputs;
  fclose(fpointer);
}

int prox_rotulo() { return proximo_rotulo_contador++; }

char *proximo_rotulo() {
  char *rotulo = (char *)malloc(
      10); // Alocando espaço suficiente (você pode ajustar conforme necessário)
  printf(rotulo, "L%d", proximo_rotulo_contador++);
  return rotulo;
}

char *rotulo_anterior() {
  char *rotulo = (char *)malloc(
      10); // Alocando espaço suficiente (você pode ajustar conforme necessário)
  sprintf(rotulo, "L%d", proximo_rotulo_contador--);
  return rotulo;
}
struct tabela *minhaTabela;
int main() {
  minhaTabela = newTabela();
  leitura("entrada.txt");
  // printf("Analisando:\n");
  printf("\nINPP");
  InfoAtomo = obter_atomo();
  lookahead = InfoAtomo.atomo;

  busca(minhaTabela);
  programa(); // chama o simbolo inicial da gramatica
  consome(EOS);
  // printf("\n...........");
  // printf("\n %.2d linhas analisadas, programa sintaticamente
  // correto",InfoAtomo.linha);
  printf("\nPARA");
  return 0;
}

//###############################
// ANALISADOR LEXICO
//###############################

TInfoAtomo obter_atomo() {
  TInfoAtomo infoAtomo;
  int linhaInicio = 0;
  // descartando os delimitadores
  while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t' ||
         *buffer == '\r') {
    if (*buffer == '\n')
      contaLinha++;
    buffer++;
  }
  // Tratando comentarios simples "//"
  if (*buffer == '/' && *(buffer + 1) == '/') {
    buffer += 2;
    infoAtomo.atomo = COMENTARIO;
    while (*buffer != '\n' && *buffer != '\0') {
      buffer++;
    }
    // Tratando comentarios de uma ou mais linhas "/**/"
  } else if (*buffer == '/' && *(buffer + 1) == '*') {
    buffer += 2;
    infoAtomo.atomo = COMENTARIO;
    while (!(*buffer == '*' && *(buffer + 1) == '/')) {
      if (*buffer == '\n')
        linhaInicio++;
      else if (*buffer == '\0')
        break;
      buffer++;
    }
    buffer += 2;
  }
  // reconhece numero
  else if (isdigit(*buffer)) { // Ve se é digito
    infoAtomo = reconhece_numero();
  }
  // reconhece identificador - trata palavras reservadas
  else if (isalpha(*buffer)) {
    infoAtomo = reconhece_id();
  } else if (*buffer == '+') {
    infoAtomo.atomo = OP_SOMA;
    buffer++;
  } else if (*buffer == '*') {
    infoAtomo.atomo = OP_MULT;
    buffer++;
  } else if (*buffer == '-') {
    infoAtomo.atomo = OP_SUBTRACAO;
    buffer++;
  } else if (*buffer == ';') {
    infoAtomo.atomo = PONTO_VIRGULA;
    buffer++;
  } else if (*buffer == '.') {
    infoAtomo.atomo = PONTO;
    buffer++;
  } else if (*buffer == ',') {
    infoAtomo.atomo = VIRGULA;
    buffer++;
  } else if (*buffer == '<') {
    buffer++;
    if (*buffer == '=') {
      infoAtomo.atomo = MENOR_IGUAL;
      buffer++;
    } else {
      infoAtomo.atomo = MENOR;
    }
  } else if (*buffer == '>') {
    buffer++;
    if (*buffer == '=') {
      infoAtomo.atomo = MAIOR_IGUAL;
      buffer++;
    } else {
      infoAtomo.atomo = MAIOR;
    }
  } else if (*buffer == '=') {
    infoAtomo.atomo = IGUAL;
    buffer++;
  } else if (*buffer == '#') {
    infoAtomo.atomo = HASHTAG;
    buffer++;
  } else if (*buffer == '(') {
    infoAtomo.atomo = ABRE_PARENTESES;
    buffer++;
  } else if (*buffer == ')') {
    infoAtomo.atomo = FECHA_PARENTESES;
    buffer++;
  } else if (*buffer == ':') {
    buffer++;
    if (*buffer == '=') {
      infoAtomo.atomo = ATRIBUICAO;
      buffer++;
    } else
      infoAtomo.atomo = DOIS_PONTOS;
  } else if (*buffer == '\x0') {
    infoAtomo.atomo = EOS;
  } else
    infoAtomo.atomo = ERRO;

  infoAtomo.linha = contaLinha;
  contaLinha += linhaInicio;

  return infoAtomo;
}

TInfoAtomo reconhece_numero() { // funcao reconhe o atomo NUMERO
  char *pIniNum = buffer;
  TInfoAtomo infoAtomo;
  infoAtomo.atomo = ERRO;

  if (isdigit(*buffer)) {
    buffer++;
    goto q1;
  }
  infoAtomo.atomo = NUMERO;
  return infoAtomo;

q1:
  if (isdigit(*buffer)) {
    buffer++;
    goto q1;
  }
  if (*buffer == 'E' || *buffer == 'e') {
    buffer++;
    goto q2;
  }
  // Se for letra ou outro simbolo que nao pertence ao alfabeto do analisador
  // lexico
  strncpy(infoAtomo.atributo_ID, pIniNum, buffer - pIniNum);
  infoAtomo.atributo_ID[buffer - pIniNum] = '\x0';
  //infoAtomo.atributo_numero = infoAtomo.atributo_ID;
  infoAtomo.atomo = NUMERO;
  return infoAtomo;

q2:
  if (*buffer == '+' || *buffer == '-') {
    buffer++;
  }
  if (isdigit(*buffer)) {
    buffer++;
    goto q3;
  }
  return infoAtomo;
q3:
  if (isdigit(*buffer)) {
    buffer++;
    goto q3;
  }
  // if (isalpha(*buffer))
  // return infoAtomo;
  // q4:
  //   if (isdigit(*buffer)) {
  //     buffer++;
  //     goto q4;
  //   }

  strncpy(infoAtomo.atributo_ID, pIniNum, buffer - pIniNum);
  infoAtomo.atributo_ID[buffer - pIniNum] = '\x0';
  //infoAtomo.atributo_numero = infoAtomo.atributo_ID;
  infoAtomo.atomo = NUMERO;
  return infoAtomo;
}

// funcao reconhe o atomo IDENTIFICADOR
TInfoAtomo reconhece_id() {
  char *pIniID = buffer;
  // size_t tamanho;
  TInfoAtomo infoAtomo;
  infoAtomo.atomo = ERRO;
  int tamanhoIdentificador = 0;

  if (isalpha(*buffer)) {
    buffer++;
    tamanhoIdentificador++;
    goto q1;
  }
  return infoAtomo;

q1:
  if (isalpha(*buffer) || isdigit(*buffer) || *buffer == '_') {
    buffer++;
    tamanhoIdentificador++;
    goto q1;
  }

  if (tamanhoIdentificador > 15) {
    return infoAtomo;
  }

  strncpy(infoAtomo.atributo_ID, pIniID, buffer - pIniID);
  // printf("\npIniID = {%s}", pIniID);
  // printf("\natributo = {%s}", infoAtomo.atributo_ID);
  infoAtomo.atributo_ID[buffer - pIniID] = '\x0';
  // printf("\natributo = {%s}", infoAtomo.atributo_ID);
  if (strcmp(infoAtomo.atributo_ID, "algoritmo") == 0)
    infoAtomo.atomo = ALGORITMO;
  else if (strcmp(infoAtomo.atributo_ID, "variavel") == 0)
    infoAtomo.atomo = VARIAVEL;
  else if (strcmp(infoAtomo.atributo_ID, "inteiro") == 0)
    infoAtomo.atomo = INTEIRO;
  else if (strcmp(infoAtomo.atributo_ID, "logico") == 0)
    infoAtomo.atomo = LOGICO;
  else if (strcmp(infoAtomo.atributo_ID, "inicio") == 0)
    infoAtomo.atomo = INICIO;
  else if (strcmp(infoAtomo.atributo_ID, "fim") == 0)
    infoAtomo.atomo = FIM;
  else if (strcmp(infoAtomo.atributo_ID, "se") == 0)
    infoAtomo.atomo = SE;
  else if (strcmp(infoAtomo.atributo_ID, "entao") == 0)
    infoAtomo.atomo = ENTAO;
  else if (strcmp(infoAtomo.atributo_ID, "senao") == 0)
    infoAtomo.atomo = SENAO;
  else if (strcmp(infoAtomo.atributo_ID, "enquanto") == 0)
    infoAtomo.atomo = ENQUANTO;
  else if (strcmp(infoAtomo.atributo_ID, "faca") == 0)
    infoAtomo.atomo = FACA;
  else if (strcmp(infoAtomo.atributo_ID, "leia") == 0)
    infoAtomo.atomo = LEIA;
  else if (strcmp(infoAtomo.atributo_ID, "escreva") == 0)
    infoAtomo.atomo = ESCREVA;
  else if (strcmp(infoAtomo.atributo_ID, "ou") == 0)
    infoAtomo.atomo = OU;
  else if (strcmp(infoAtomo.atributo_ID, "e") == 0)
    infoAtomo.atomo = E;
  else if (strcmp(infoAtomo.atributo_ID, "div") == 0)
    infoAtomo.atomo = DIV;
  else if (strcmp(infoAtomo.atributo_ID, "verdadeiro") == 0)
    infoAtomo.atomo = VERDADEIRO;
  else if (strcmp(infoAtomo.atributo_ID, "falso") == 0)
    infoAtomo.atomo = FALSO;
  else
    infoAtomo.atomo = IDENTIFICADOR;
  return infoAtomo;
}

//###############################
// ANALISADOR SINTATICO
//###############################

void consome(TAtomo atomo) {
  if (lookahead == atomo) {
    // printf("\n#  %.2d:%s", InfoAtomo.linha, strAtomo[lookahead]);
    //  if (InfoAtomo.atomo == IDENTIFICADOR || InfoAtomo.atomo == NUMERO)
    //    printf(" - ATRIBUTO: %s", InfoAtomo.atributo_ID);
    InfoAtomo = obter_atomo();
    lookahead = InfoAtomo.atomo;
    while (lookahead == COMENTARIO)
      consome(COMENTARIO);
  } else {
    printf("\n...........");
    printf("\n#%.2d: Erro sintatico: esperado [%s] encontrado [%s]\n",
           (InfoAtomo.linha), strAtomo[atomo], strAtomo[lookahead]);
    exit(1);
  }
}

void programa() {
  while (lookahead == COMENTARIO)
    consome(COMENTARIO);
  consome(ALGORITMO);
  consome(IDENTIFICADOR);
  consome(PONTO_VIRGULA);
  bloco();
  consome(PONTO);
  while (lookahead == COMENTARIO) // Apos ler "." pode-se ler comentarios
    consome(COMENTARIO);
}

void bloco() {
  if (lookahead == VARIAVEL)
    declaracao_var();
  comando_composto();
}

void declaracao_var() {

  consome(VARIAVEL);

  do {
    lista_var(1);
    consome(DOIS_PONTOS);
    tipo();
    consome(PONTO_VIRGULA);
  } while (lookahead == IDENTIFICADOR);
}

void lista_var(int flag) {
  if (flag == 1) {
    adicionaNo();
    consome(IDENTIFICADOR);
    int contador = 0;
    while (lookahead == VIRGULA) {
      contador++;
      consome(VIRGULA);
      adicionaNo();
      consome(IDENTIFICADOR);
    };
    printf("\nAMEM %d", ++contador);
  } else {
    if (busca(minhaTabela) == -1) {
      erroSemantico();
    }
    consome(IDENTIFICADOR);
    int contador = 0;
    while (lookahead == VIRGULA) {
      contador++;
      consome(VIRGULA);
      if (busca(minhaTabela) == -1) {
        erroSemantico();
      }
      consome(IDENTIFICADOR);
    };
  }
  // printf("\n\ntamanho: %d", minhaTabela->tamanho);
}

void tipo() {
  if (lookahead == INTEIRO)
    consome(INTEIRO);
  else //(lookahead == LOGICO)
    consome(LOGICO);
}

void comando_composto() {
  consome(INICIO);
  comando();

  while (lookahead == PONTO_VIRGULA) {
    consome(PONTO_VIRGULA);
    comando();
  }

  consome(FIM);
  while (lookahead == COMENTARIO)
    consome(COMENTARIO);
}

void comando() {
  if (lookahead == IDENTIFICADOR)
    comando_atribuicao();
  else if (lookahead == SE)
    comando_se();
  else if (lookahead == ENQUANTO)
    comando_enquanto();
  else if (lookahead == LEIA)
    comando_entrada();
  else if (lookahead == ESCREVA)
    comando_saida();
  else //(lookahead == INICIO){}
    comando_composto();
}

void comando_enquanto() { // Exemplo Luba
  int L1 = prox_rotulo();
  int L2 = prox_rotulo();

  consome(ENQUANTO);
  printf("\nL%d:\tNADA", L1);

  consome(ABRE_PARENTESES);
  expressao();
  consome(FECHA_PARENTESES);

  printf("\nDSVF L%d", L2);
  consome(FACA);
  comando();
  printf("\nDSVS L%d", L1);

  printf("\nL%d:\tNADA", L2);
}

void comando_entrada() {
  consome(LEIA);
  consome(ABRE_PARENTESES);
  lista_var(0);
  consome(FECHA_PARENTESES);
  printf("\nLEIT");
  printf("\nARMZ %d", busca(minhaTabela));
}

void comando_saida() {
  // printf("\nIMPR");
  int *key;
  consome(ESCREVA);
  consome(ABRE_PARENTESES);
  expressao(key);
  while (lookahead == VIRGULA) {
    consome(VIRGULA);
    expressao(key);
  }
  consome(FECHA_PARENTESES);
  printf("\nIMPR");
}

void comando_atribuicao() {
  int *key;
  int res = busca(minhaTabela);
  if (res == -1) {
    erroSemantico();
  } else {
    if (busca(minhaTabela) == -1) {
      erroSemantico();
    }
    consome(IDENTIFICADOR);
    consome(ATRIBUICAO);
    expressao(key);
    printf("\nARMZ %d", res);
  }
}

void comando_se() {
  int L1 = prox_rotulo();
  int L2 = prox_rotulo();
  consome(SE);
  consome(ABRE_PARENTESES);
  expressao();
  consome(FECHA_PARENTESES);
  consome(ENTAO);
  printf("\nDSVF L%d", L1);
  comando();
  printf("\nDSVS L%d", L2);
  printf("\nL%d:\tNADA", L1);
  if (lookahead == SENAO) {
    consome(SENAO);
    comando();
  }
  printf("\nL%d:\tNADA", L2);
}

void expressao() {
  char aux[100];
  strcpy(aux, InfoAtomo.atributo_ID);
  expressao_simples();
  // printf("\n%s", aux);
  if (lookahead == MENOR || lookahead == MENOR_IGUAL || lookahead == IGUAL ||
      lookahead == HASHTAG || lookahead == MAIOR || lookahead == MAIOR_IGUAL) {
    TAtomo aux = lookahead;
    relacional();
    expressao_simples();
    switch (aux) {
    case MENOR:
      printf("\nCMME");
      break;
    case MENOR_IGUAL:
      printf("\nCMEG");
      break;
    case IGUAL:
      printf("\nCMIG");
      break;
    case HASHTAG:
      printf("\nCMDG");
      break;
    case MAIOR:
      printf("\nCMMA");
      break;
    case MAIOR_IGUAL:
      printf("\nCMAG");
      break;
    default:
      break;
    }
  }
}

void relacional() {
  if (lookahead == MENOR)
    consome(MENOR);
  else if (lookahead == MENOR_IGUAL)
    consome(MENOR_IGUAL);
  else if (lookahead == IGUAL)
    consome(IGUAL);
  else if (lookahead == HASHTAG)
    consome(HASHTAG);
  else if (lookahead == MAIOR)
    consome(MAIOR);
  else // (lookahead == MAIOR_IGUAL)
    consome(MAIOR_IGUAL);
}

void expressao_simples() {
  int key = 0;
  if (lookahead == OP_SOMA) {
    // printf("\nSOMA");
    consome(OP_SOMA);
    key = 1;
  } else if (lookahead == OP_SUBTRACAO) {
    // printf("\nSUBT");
    consome(OP_SUBTRACAO);
    key = 2;
  }

  termo(&key);
  if (key == 1) {
    printf("\nSOMA");
  } else if (key == 2) {
    printf("\nSUBT");
  } else if (key == 3) {
    printf("\nMULT");
  } else if (key == 4) {
    printf("\nDIVI");
  }
  while (lookahead == OP_SOMA || lookahead == OP_SUBTRACAO || lookahead == OU) {
    if (lookahead == OP_SOMA) {

      consome(OP_SOMA);
      key = 1;

    } else if (lookahead == OP_SUBTRACAO) {

      consome(OP_SUBTRACAO);
      key = 2;

    } else
      consome(OU);
    termo(&key);
    if (key == 1) {
      printf("\nSOMA");
    } else if (key == 2) {
      printf("\nSUBT");
    } else if (key == 3) {
      printf("\nMULT");
    } else if (key == 4) {
      printf("\nDIVI");
    }
  }
}

void termo(int *key) {
  fator();
  while (lookahead == OP_MULT || lookahead == DIV || lookahead == E) {
    if (lookahead == OP_MULT) {

      consome(OP_MULT);
      *key = 3;
      // printf("\nMULT");

    } else if (lookahead == DIV) {

      consome(DIV);
      *key = 4;
      // printf("\nDIV");
    } else //(lookahead == E)
      consome(E);
    fator();
  }
}

void fator() {
  int *key = 0;
  if (lookahead == IDENTIFICADOR) {
    int endereco = busca(minhaTabela);
    if (busca(minhaTabela) == -1) {
      erroSemantico();
    }
    printf("\nCRVL %d", endereco);
    consome(IDENTIFICADOR);
  }

  else if (lookahead == NUMERO) {
    printf("\nCRCT %s", InfoAtomo.atributo_ID);
    consome(NUMERO);
  }
  // Como não vamos trabalhar com valores booleanos, acho que não precisamos
  // dessa parte
  else if (lookahead == VERDADEIRO)
    consome(VERDADEIRO);
  else if (lookahead == FALSO)
    consome(FALSO);
  else { // (lookahead == ABRE_PARENTESES) {
    consome(ABRE_PARENTESES);
    expressao();
    consome(FECHA_PARENTESES);
  }
}

int busca(struct tabela *minhaTabela) {
  return busca_tabela_simbolos(minhaTabela, InfoAtomo.atributo_ID);
}

void adicionaNo() {
  int res = addNode(minhaTabela, InfoAtomo.atributo_ID);
  if (res == 1) {
    return;
  } else {
    erroSemantico();
  }
}

void erroSemantico() {
  printf("\nErro semantico");
  exit(1);
}