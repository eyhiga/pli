#ifndef DAG_HPP
#define DAG_HPP

#define MAXIMO_VERTICES 400
#define PESO_MAX 100

struct DAG {
	int n; // número de vértices
	int S[MAXIMO_VERTICES]; // peso dos vértices
	int B[MAXIMO_VERTICES][MAXIMO_VERTICES]; // peso das arestas
	int D[MAXIMO_VERTICES][MAXIMO_VERTICES]; // matriz de adjacência
};

int carregaGrafo(char *arquivo, DAG *dag);
void salvaGrafo(char *arquivo, DAG *dag);
void sorteiaPesosGrafo(DAG *dag);

#endif
