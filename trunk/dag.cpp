#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dag.hpp"

#define TAMANHO_LINHA 100

int carregaGrafo(char *arquivo, DAG *dag){
	FILE *arquivoGrafo = fopen (arquivo, "r");
	char linha[TAMANHO_LINHA];
	int i,j;

	// Lê n (número de vértices)
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "n:") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}
	fscanf(arquivoGrafo, "%d", &(dag->n));

	// Ignora I (I?)
	fgets(linha, TAMANHO_LINHA, arquivoGrafo);
	fgets(linha, TAMANHO_LINHA, arquivoGrafo);
	
	// Lê S (peso dos vértices?)
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "S:") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}

	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "[") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}
	
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "(1)") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}

	for (i=0; i<dag->n; i++) { // lê elementos
		fscanf(arquivoGrafo, "%d", &(dag->S[i]));
	}
	
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "]") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}

	// Lê B (peso das arestas)
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "B:") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}

	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "[") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}
	for (i=0; i<dag->n; i++) { // lê cada linha da matriz
		fscanf(arquivoGrafo, "%s", linha);
		fscanf(arquivoGrafo, "%s", linha);
		if (strcmp(linha, "1)") != 0) { // erro de parse
			fclose(arquivoGrafo);
			return -1;
		}
		for (j=0; j<dag->n; j++) {
			fscanf(arquivoGrafo, "%d", &(dag->B[i][j]));
		}
	}

	fgets(linha, TAMANHO_LINHA, arquivoGrafo);
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "]") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}

	// Lê D (matriz de adjacência)
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "D:") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "[") != 0) { // erro de parse
		fclose(arquivoGrafo);
		return -1;
	}
	for (i=0; i<dag->n; i++) { // lê cada linha da matriz
		
		fscanf(arquivoGrafo, "%s", linha);
		fscanf(arquivoGrafo, "%s", linha);
		if (strcmp(linha, "1)") != 0) { // erro de parse
			fclose(arquivoGrafo);
			return -1;
		}
		
		for (j=0; j<dag->n; j++) {
			fscanf(arquivoGrafo, "%d", &(dag->D[i][j]));
		}
	}
	
	fscanf(arquivoGrafo, "%s", linha);
	if (strcmp(linha, "]") != 0) { // erro de parse
		fclose(arquivoGrafo);
		printf("hey %s\n", linha);
		return -1;
	}
	
	// Encerra leitura do arquivo
	fclose(arquivoGrafo);
}

void salvaGrafo(char *arquivo, DAG *dag) {
	FILE *arquivoGrafo = fopen (arquivo, "w");
	int i,j;

	// n
	fprintf(arquivoGrafo, "n: %d\n", dag->n);
	
	// I
	fprintf(arquivoGrafo, "I: [ (1)]\n", dag->n);
	
	// S
	fprintf(arquivoGrafo, "S: [ (1)", dag->n);
	for (i=0; i<dag->n; i++) {
		fprintf(arquivoGrafo, " %d", dag->S[i]);
	}
	fprintf(arquivoGrafo, "]\n");
	
	// B
	fprintf(arquivoGrafo, "B: [ (1 1) ", dag->n);
	for (j=0; j<dag->n; j++) {
		fprintf(arquivoGrafo, "%d ", dag->B[0][j]);
	}
	fprintf(arquivoGrafo, "\n");
	for (i=1; i<dag->n; i++) {
		fprintf(arquivoGrafo, "     (%d 1) ", i+1);
		for (j=0; j<dag->n; j++) {
			fprintf(arquivoGrafo, "%d ", dag->B[i][j]);
		}
		fprintf(arquivoGrafo, "\n");
		
	}
	fprintf(arquivoGrafo, "   ]\n");

	// D
	fprintf(arquivoGrafo, "D: [ (1 1) ", dag->n);
	for (j=0; j<dag->n; j++) {
		fprintf(arquivoGrafo, "%d ", dag->D[0][j]);
	}
	fprintf(arquivoGrafo, "\n");
	for (i=1; i<dag->n; i++) {
		fprintf(arquivoGrafo, "     (%d 1) ", i+1);
		for (j=0; j<dag->n; j++) {
			fprintf(arquivoGrafo, "%d ", dag->D[i][j]);
		}
		fprintf(arquivoGrafo, "\n");
	}
	fprintf(arquivoGrafo, "   ]\n");
	
	
	// Encerra leitura do arquivo
	fclose(arquivoGrafo);
}

void sorteiaPesosGrafo(DAG *dag) {
	int i,j;
	for (i=0; i<dag->n; i++)
	for (j=0; j<dag->n; j++) {
		if (dag->D[i][j] == 0)
			dag->B[i][j] = 0;
		else
			dag->B[i][j] = (rand() % (PESO_MAX - 1))+1; 
	}	
}