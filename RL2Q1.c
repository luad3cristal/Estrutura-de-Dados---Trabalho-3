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
	return (novo->valor < atual->valor) ? 'e' : 'd';
}

void inserir(arvore **topo, int valor, int *log) {
	arvore *novo = criarNo(valor);
	if (*topo == NULL) {
		*topo = novo;
		log[0] = novo->altura;
		return;
	}

	arvore *atual = *topo;
	while (1) {
		int comparacao = comparar(atual, novo);

		if (comparacao == 'e') {
			if (atual->esq == NULL) {
				atual->esq = novo;
				novo->altura = atual->altura + 1;
				break;
			}
			atual = atual->esq;
		} else {
			if (atual->dir == NULL) {
				atual->dir = novo;
				novo->altura = atual->altura + 1;
				break;
			}
			atual = atual->dir;
		}
	}

	for (int i = 0; i < logTam; i++) {
		if (log[i] == -1) {
			log[i] = novo->altura;
			break;
		}
	}
}

arvore *acharMax(arvore **topo) {
	if (*topo == NULL) return NULL;
	arvore *atual = *topo, *anterior = NULL;
	while (atual->dir != NULL) {
		anterior = atual;
		atual = atual->dir;
	}
	return anterior;
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
	char *inputCaminho = "./L2Q1.in";
	char *outputCaminho = "./L2Q1.out";
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
		int log[logTam];
		for (int j = 0; j < logTam; j++) log[j] = -1;

		char *token = strtok(linhas[i], " ");
		while (token) {
			int valor = atoi(token);
			inserir(&topo, valor, log);
			token = strtok(NULL, " ");
		}

		arvore *predecessorMax = acharMax(&topo);
		int maxValor = (predecessorMax && predecessorMax->dir) ? predecessorMax->dir->valor : topo->valor;
		int maxAltura = (predecessorMax && predecessorMax->dir) ? predecessorMax->dir->altura : topo->altura;
		int predValor = (predecessorMax) ? predecessorMax->valor : -1;

		char linhaOutput[1024] = "";
		for (int j = 0; j < logTam; j++) {
			if (log[j] != -1) {
				char temp[10];
				sprintf(temp, "%d ", log[j]);
				strcat(linhaOutput, temp);
			}
		}

		char temp[100];
		if (predValor != -1) {
			sprintf(temp, "max %d alt %d pred %d", maxValor, maxAltura, predValor);
		} else {
			sprintf(temp, "max %d alt %d pred NaN", maxValor, maxAltura);
		}
		if (strlen(output) > 0) strcat(output, "\n");
		strcat(output, linhaOutput);
		strcat(output, temp);
	}

	escreverArquivo(outputCaminho, output);

	free(conteudo);
	for (int i = 0; i < linhasCount; i++) {
		free(linhas[i]);
	}

	return 0;
}
