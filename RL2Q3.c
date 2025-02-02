#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1024

typedef struct Item {
  int valor;
  struct Item *dir;
  struct Item *esq;
  struct Item *pai;
} Item;

int calcAltura(Item *raiz) {
  int altura = 0;
  Item *raizAtual = raiz;
  while (raizAtual->pai != NULL) {
    altura++;
    raizAtual = raizAtual->pai;
  }
  return altura;
}

void imprimirEmOrdem(Item *raiz, char *output, int *pos) {
  if (raiz != NULL) {
    imprimirEmOrdem(raiz->esq, output, pos);
    int altura = calcAltura(raiz);
    *pos += sprintf(output + *pos, "%d (%d) ", raiz->valor, altura);
    imprimirEmOrdem(raiz->dir, output, pos);
  }
}

Item *encontrarMinimo(Item *no) {
  while (no->esq != NULL) {
    no = no->esq;
  }
  return no;
}

Item *removerNoComPai(Item *raiz, int valor, Item *pai) {
  if (raiz == NULL) {
    return raiz;
  }

  if (valor < raiz->valor) {
    raiz->esq = removerNoComPai(raiz->esq, valor, raiz);
  } else if (valor > raiz->valor) {
    raiz->dir = removerNoComPai(raiz->dir, valor, raiz);
  } else {
    if (raiz->esq == NULL) {
      Item *temp = raiz->dir;
      if (temp != NULL)
        temp->pai = pai;
      free(raiz);
      return temp;
    } else if (raiz->dir == NULL) {
      Item *temp = raiz->esq;
      if (temp != NULL)
        temp->pai = pai;
      free(raiz);
      return temp;
    }
    Item *temp = encontrarMinimo(raiz->dir);
    raiz->valor = temp->valor;
    raiz->dir = removerNoComPai(raiz->dir, temp->valor, raiz);
  }
  return raiz;
}

char *lerArquivo(const char *caminho) {
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
    perror("Erro ao alocar memória");
    exit(EXIT_FAILURE);
  }

  fread(buffer, 1, fileSize, arquivo);
  buffer[fileSize] = '\0';

  fclose(arquivo);
  return buffer;
}

void escreverArquivo(const char *caminho, char *data) {
  FILE *arquivo = fopen(caminho, "w");
  if (!arquivo) {
    perror("Erro ao abrir o arquivo de saída");
    exit(EXIT_FAILURE);
  }

  fprintf(arquivo, "%s", data);
  fclose(arquivo);
  printf("Arquivo de saída atualizado com sucesso!\n");
}

void inserir(Item **raiz, int num) {
  if (*raiz == NULL) {
    *raiz = (Item *)malloc(sizeof(Item));
    if (!(*raiz)) {
      perror("Erro ao alocar memória");
      exit(EXIT_FAILURE);
    }
    (*raiz)->valor = num;
    (*raiz)->esq = NULL;
    (*raiz)->dir = NULL;
    (*raiz)->pai = NULL;
    return;
  }

  Item *atual = *raiz, *pai = NULL;
  while (atual != NULL) {
    pai = atual;
    if (num < atual->valor)
      atual = atual->esq;
    else
      atual = atual->dir;
  }

  Item *novo = (Item *)malloc(sizeof(Item));
  if (!novo) {
    perror("Erro ao alocar memória");
    exit(EXIT_FAILURE);
  }
  novo->valor = num;
  novo->esq = NULL;
  novo->dir = NULL;
  novo->pai = pai;

  if (num < pai->valor)
    pai->esq = novo;
  else
    pai->dir = novo;
}

int main() {
  const char *inputCaminho = "./L2Q3.in";
  const char *outputCaminho = "./L2Q3.out";

  char *fileContent = lerArquivo(inputCaminho);

  char *linhas[MAX_LINES];
  int linhasCount = 0;

  char *tokenLinha = strtok(fileContent, "\n");
  while (tokenLinha) {
    linhas[linhasCount] = malloc(strlen(tokenLinha) + 1);
    if (!linhas[linhasCount]) {
      perror("Erro ao alocar memória");
      exit(EXIT_FAILURE);
    }
    strcpy(linhas[linhasCount], tokenLinha);
    linhasCount++;
    tokenLinha = strtok(NULL, "\n");
  }

  char outputBuffer[MAX_LINES * MAX_LINE_LENGTH] = "";
  int pos = 0;

  for (int i = 0; i < linhasCount; i++) {
    char *linha = linhas[i];
    int tam = strlen(linha);
    Item *raiz = NULL;
    char lastOp = '\0';

    for (int j = 0; j < tam; j++) {
      if (linha[j] == 'a' || linha[j] == 'r') {
        lastOp = linha[j];
        continue;
      }
      if (linha[j] == ' ')
        continue;

      if (isdigit(linha[j]) ||
          (linha[j] == '-' && j + 1 < tam && isdigit(linha[j + 1]))) {
        char valor[20];
        int l = 0;
        while (j < tam && linha[j] != ' ' && linha[j] != 'a' &&
               linha[j] != 'r') {
          valor[l++] = linha[j++];
        }
        valor[l] = '\0';
        j--;

        int num = atoi(valor);

        if (lastOp == 'a') {
          inserir(&raiz, num);
        } else if (lastOp == 'r') {
          Item *atual = raiz;
          Item *pai = NULL;
          while (atual != NULL) {
            if (num == atual->valor)
              break;
            pai = atual;
            if (num < atual->valor)
              atual = atual->esq;
            else
              atual = atual->dir;
          }
          if (atual != NULL) {
            if (pai == NULL) {
              raiz = removerNoComPai(raiz, num, NULL);
            } else if (pai->esq == atual) {
              pai->esq = removerNoComPai(pai->esq, num, pai);
            } else {
              pai->dir = removerNoComPai(pai->dir, num, pai);
            }
          } else {
            inserir(&raiz, num);
          }
        }
      }
    }
    imprimirEmOrdem(raiz, outputBuffer, &pos);
    if (pos > 0)
      outputBuffer[pos - 1] = '\n';
  }

  if (pos > 0)
    outputBuffer[pos - 1] = '\0';

  escreverArquivo(outputCaminho, outputBuffer);

  free(fileContent);
  for (int i = 0; i < linhasCount; i++) {
    free(linhas[i]);
  }

  return 0;
}
