#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dag.hpp"
#include "grid.hpp"

#define NOVA_SEMENTE "NOVA_SEMENTE"

#define ARQUIVO_MONTAGE "DAGs/montage_10_app.dat"
#define ARQUIVO_SIMPLE "DAGs/simple_100_app.dat"
#define ARQUIVO_WIEN2K "DAGs/wien2k_100_app.dat"

#define MONTAGE 0
#define SIMPLE 1
#define WIEN2K 2

#define TIPOS_GRAFOS 3

/*
 * Sorteia um número entre 0 e TIPOS_GRAFOS que represente
 * um dos grafos da aplicação
 */
int sorteiaGrafo() {
	return rand() % TIPOS_GRAFOS;
}

/*
 * Parametro 1 - semente usada
 * Caso seja passado NOVA_SEMENTE, então usa tempo
 * corrente como semente
 */
int main(int argc, char *argv[]) {
	int simulando = 1;
	int tempo = 0;
	int nrequisicoes = 0;
	char arquivoDag[100];
	DAG dag;
	Grid grid;
  
	/* Inicia gerador aleatórios */
	unsigned int seed;
	if (strcmp(argv[1],NOVA_SEMENTE) == 0) { // deve usar nova semente
		time_t tempo = time(NULL);
		seed = tempo;
	} else {
		seed = atol(argv[3]);
	}
	srand(seed);
	printf("Semente usada: %u\n", seed);

	// Carrega configuração da rede do grid
	// TODO
	
	while (simulando) {
		int intervalo = 3; // tempo de diferença que cehegará a próxima requisição
		int dagEscolhido;
		tempo += intervalo;
		
		/* Sorteia nova requisição */
		dagEscolhido = sorteiaGrafo();
		switch (dagEscolhido) {
			case MONTAGE:
				strcpy(arquivoDag, ARQUIVO_MONTAGE);
				break;
			case SIMPLE:
				strcpy(arquivoDag, ARQUIVO_SIMPLE);
				break;
			case WIEN2K:
				strcpy(arquivoDag, ARQUIVO_WIEN2K);
				break;
		}

		/* Carrega DAG */
		carregaGrafo(arquivoDag, &dag);
		sorteiaPesosGrafo(&dag);

		/* Chama escalonador */
		
		/* Calcula novo estado da rede */
		
		/* Atualiza requisições recebidas e verifica se continua simulação */
		nrequisicoes++;
		simulando = (nrequisicoes < 10);
	}
}