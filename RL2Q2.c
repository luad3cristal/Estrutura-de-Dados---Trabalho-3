#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define logTam 50000

typedef struct arvore {
  int valor;
  int altura;
  struct arvore *esq, *dir;
} arvore;

arvore *criarNo(int valor) {
  arvore *novo = (arvore *)malloc(sizeof(arvore));
  novo->valor = valor;
  novo->altura = 0;
  novo->esq = novo->dir = NULL;
  return novo;
}

char comparar(arvore *atual, arvore *novo) {
  if (novo->valor != atual->valor) {
    return (novo->valor < atual->valor) ? 'e' : 'd';
  }
  return 'i';
}

void inserir(arvore **topo, int valor) {
  arvore *novo = criarNo(valor);
  if (*topo == NULL) {
    *topo = novo;
    return;
  }

  arvore *atual = *topo;
  while (1) {
    char comparacao = comparar(atual, novo);

    if (comparacao == 'e') {
      if (atual->esq == NULL) {
        atual->esq = novo;
        novo->altura = atual->altura + 1;
        break;
      }
      atual = atual->esq;
    } 
    else if (comparacao == 'd') {
      if (atual->dir == NULL) {
        atual->dir = novo;
        novo->altura = atual->altura + 1;
        break;
      }
      atual = atual->dir;
    }
    else {
      free(novo);
      break;
    }
  }
}

void percorrerEsq(arvore *topo, int *somaEsq) {
  if (topo == NULL) return;

  *somaEsq += topo->valor;
  percorrerEsq(topo->esq, somaEsq);
  percorrerEsq(topo->dir, somaEsq);
}

void percorrerDir(arvore *topo, int *somaDir) {
  if (topo == NULL) return;

  *somaDir += topo->valor;
  percorrerDir(topo->dir, somaDir);
  percorrerDir(topo->esq, somaDir);
}

void emOrdem(arvore **topo, char *log) {
  if (*topo != NULL) {
    emOrdem(&((*topo)->esq), log);

    int somaEsq = 0, somaDir = 0, total = 0;

    percorrerEsq((*topo)->esq, &somaEsq);
    percorrerDir((*topo)->dir, &somaDir);

    total = somaDir - somaEsq;

    char buffer[50];
    sprintf(buffer, "%d (%d)", (*topo)->valor, total); 
    if (strlen(log) > 0) {
        strcat(log, " "); 
    }
    strcat(log, buffer);  

    emOrdem(&((*topo)->dir), log);
  }
}


char *lerArquivo(char *caminho) {
  FILE *arquivo = fopen(caminho, "r");
  if (!arquivo) {
      perror("Erro ao abrir o arquivo de entrada");
      exit(EXIT_FAILURE);
  }

  fseek(arquivo, 0, SEEK_END);
  long fileSize = ftell(arquivo);
  rewind(arquivo);

  char *buffer = (char *)malloc((fileSize + 1) * sizeof(char));
  if (!buffer) {
      perror("Erro ao alocar memória para o buffer");
      exit(EXIT_FAILURE);
  }
  fread(buffer, 1, fileSize, arquivo);
  buffer[fileSize] = '\0';

  fclose(arquivo);
  return buffer;
}

void escreverArquivo(char *caminho, char *data) {
  FILE *arquivo = fopen(caminho, "w");
  if (!arquivo) {
      perror("Erro ao abrir o arquivo de saída");
      exit(EXIT_FAILURE);
  }
  fprintf(arquivo, "%s", data);
  fclose(arquivo);
}

int main() {
  char *inputCaminho = "./L2Q2.in";
  char *outputCaminho = "./L2Q2.out";
  char *conteudo = lerArquivo(inputCaminho);

  char *linhas[1000];
  int linhasCount = 0;
  char *linha = strtok(conteudo, "\r\n");
  while (linha) {
      linhas[linhasCount++] = strdup(linha);
      linha = strtok(NULL, "\r\n");
  }

  char output[500000] = "";
  for (int i = 0; i < linhasCount; i++) {
      arvore *topo = NULL;

      char *token = strtok(linhas[i], " ");
      while (token) {
          int valor = atoi(token);
          inserir(&topo, valor);
          token = strtok(NULL, " ");
      }

      char log[logTam] = "";
      emOrdem(&topo, log);

      if (strlen(output) > 0) strcat(output, "\n");
      strcat(output, log);
  }

  escreverArquivo(outputCaminho, output);

  free(conteudo);
  for (int i = 0; i < linhasCount; i++) {
      free(linhas[i]);
  }

  return 0;
}