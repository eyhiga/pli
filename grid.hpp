#ifndef HOSTS_HPP
#define HOSTS_HPP

#define MAXIMO_HOSTS 500

struct Grid {
	int m; // número de máquinas na grade
	float TI[MAXIMO_HOSTS]; // tempo que u cada host demora para processar uma instrução
	int C[MAXIMO_HOSTS]; // número de núcleos (cores) disponível por máquina
	
	// matriz com os enlaces entre as máquinas (N[x][y]=1 se há enlace entre x e y, N[x][y]=0 caso contrário
	int N[MAXIMO_HOSTS][MAXIMO_HOSTS]; 
	
	// matriz com os valores de tempo para transmitir um bit entre dois hosts
	// Valores de interesse TB[x][y] estão presentes para valores de x e y tais que N[x][y]=1
	float TB[MAXIMO_HOSTS][MAXIMO_HOSTS];
};

/*
 * Encontra o índice da máquina mais lenta em termos de processamento
 * no grid.
 */
int hostMaisLento(Grid *grid);

#endif
