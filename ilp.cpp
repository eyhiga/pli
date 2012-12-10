/*
   CPLEX.cpp
*/

#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include "ilcplex/ilocplex.h"
#include "ilconcert/iloxmlcontext.h"

#include "dag.hpp"
#include "grid.hpp"

using namespace std;

//
// Integer Programming formulation 
//

/** Os parámetros desta função serão as referencias das estruturas de datos que contém a informação da simulação. Para este exemplo, suponha
que parametros é um punteiro a uma estrutura que contém toda a informação que precisa esta modelagem em particular. 
 
    Também é possível parametrizar outras coisas, como bandeiras para determinar quais restrições a ser usadas ou parametros para determinar se alguma heuristicas vai ser usada. **/

int ILP(DAG * dag, Grid * grid, int tMax, float alpha, float beta[MAXIMO_HOSTS][MAXIMO_HOSTS],
float gama, float lambda, string algoritmo) {

	//IloBoolVarArray é a estrutura para guardar as variaveis do problema. São estruturas do mesmo CPLEX, use elas ao inves de matrices/punteiros do C

	typedef IloArray < IloBoolVarArray > IloBoolVarArray2; 
	typedef IloArray < IloBoolVarArray2 > IloBoolVarArray3;
	typedef IloArray < IloBoolVarArray3 > IloBoolVarArray5;
	typedef IloArray < IloBoolVarArray4 > IloBoolVarArray4;

	// Para a variavel inteira, usem IloIntVarArray

	typedef IloArray < IloIntVarArray > IloIntVarArray2; 
	typedef IloArray < IloIntVarArray2 > IloIntVarArray3;
	typedef IloArray < IloIntVarArray3 > IloIntVarArray5;
	typedef IloArray < IloIntVarArray4 > IloIntVarArray4;


	IloEnv env;
	IloModel model(env);

	// Definição das variáveis

	/* X, variável usada para definir os momentos de término de cada tarefa x */
	IloBoolVarArray3 X(env, dag->n);
	for (int i = 0; i < dag->n; i++) {
		X[i]  = IloBoolVarArray2(env, tMax);
		for (int j = 0; j < tMax; j++) {
			X[i][j]  = IloBoolVarArray(env, grid->m);
		}
	}
   
	/* Y, indica transferência de dados entre duas tarefas
	TODO verificar limites de tamanho, pois nem todas tarefas
	possuem dependências */
	IloBoolVarArray4 Y(env, dag->n);
	for (int i = 0; i < dag->n; i++) {
		Y[i] = IloBoolVarArray3(env, dag->n);
		for(int j = 0; j < dag->n, j++)
		{
			Y[i][j] = IloBoolVarArray2(env, grid->m);
			for(int k = 0; k < grid->m; k++)
			{
				Y[i][j][k] = IloBoolVarArray(env, grid->m);
			}
		}
	}
   
	/* U, núcleos (cores) em uso de máquina em determinado tempo */
	IloBoolVarArray2 U(env, grid->m);
	for(int i=0; i < grid->m, i++)
	{
		U[i] = IloBoolVarArray(env, tMax);
	}

	/* P, indica se chassi está em uso */
	IloBoolVarArray2 P(env, grid->m);
	for(int i=0; i < grid->m, i++)
	{
		P[i] = IloBoolVarArray(env, tMax);
	}

	/* F, Indica se uma tarefa foi executada em determinada máquina */
	IloBoolVarArray2 F(env, dag->n);
	for(int i=0; i < dag->n, i++)
	{
		F[i] = IloBoolVarArray(env, grid->m);
	}

	/* Função Objetivo */
	IloExpr expr_objetivo(env);

	/* Parte do consumo por máquina */
	for(int k=0; k<grid->m; k++)
	{
		for(int t=0; t<dag->n; t++)
		{
			expr_objetivo += alpha * P[k][t] + lambda * U[k][t];
		}
	}

	/* Parte do consumo dos enlaces
	 * TODO: mesmo comentário que está na variável Y
	 * TODO: TEMPO_TRANSMISSAO, NUMERO_DE_BYTES, BETA
	 */
	for(int k=0; k<grid->m; k++)
	{
		for(int l=0; l<grid->m; j++)
		{
			for(int i=0; i<dag->n; i++)
			{
				for(int j=0; j<dag->n; j++)
				{
					expr_objetivo += 8 * *TEMPO_TRANSMISSAO[k][l]* * *NUMERO_DE_BYTES[i][j]* * y[i][j][k][l] * (2 * gama + *beta[k][l]*)
				}
			}
		}
	}

	IloObjective obj = IloMinimize(env, expr_objetivo);
	model.add(obj);

	/************************************************
	                    RESTRICOES
	*************************************************/
	// Restricao 1
	for(int j = 0; j < dag->n; j++) {
		IloExpr expr_restricao1(env);
		for(int t = 0; t < tMax; t++) {
			for(int k = 0; k < grid->m; k++) {
				expr_restricao1 += X[j][t][k];
			}
		}
		model.add(expr_restricao1 == 1);
	}

	// Restricao 2
	for(int j=0; j < dag->n; j++)
	{
		for(int k=0; k < grid->m; k++)
		{
			for(int t=1; t < ceil(I[j] * grid->TI[k]); t++)
			{
				IloExpr expr_restricao2(env);
				expr_restricao2 = X[j][t][k];
				model.add(expr_restricao2 = 0);
			}
		}
	}

	// Restricao 3
	// TODO fazer
  
	// Restricao 4
	// TODO Ajuda!
	for(int k=0; k < grid->m; k++)
	{
		for(int t=0; t < ceil(Tmax - dag->S[j] * grid->TI[k]); t++)
		{
			IloExpr expr_restricao4(env);
			for(int j=0; j < dag->n; j++)
			{
				for(int s=1; s < ceil(t-dag->S[j] * grid->TI[l] - B[i][j] * TK[k][l]))
				{
					expr_restricao4 += X[i][s][k];
				}
			}
			model.add(expr_restricao4 <= C[k]);
		}
	}

	// Restricao 5
	for(int k=0; k < grid->m; k++)
	{
		for(int t=0; t < tMax; t++)
		{
			IloExpr expr_restricao5(env);
			expr_restricao5 = U[k][t] - P[k][t];
			model.add(expr_restricao5 >= 0);
		}
	}

	// Restricao 6
	for(int k=0; k < grid->m; k++)
	{
		for(int t=0; t < tMax; t++)
		{
			IloExpr expr_restricao6(env);
			expr_restricao6 = P[k][t] - (U[k][t] / C[k]);
			model.add(expr_restricao6 >= 0);
		}
	}

	// Restricao 7
	for(int k=0; k < grid->m; k++)
	{
		for(int t=0; t < tMax; t++)
		{
			IloExpr expr_restricao7(env);

			for(int j=0; j < dag->n; j++)
			{
				for(int s=t; s < t + dag->S[j] * grid->TI[k] - 1; s++)
				{
					expr_restricao7 += X[j][s][k];
				}
			}
			model.add(U[k][t] = expr_restricao7);
		}
	}


	// Restricao 8
	for(int i=0; i < dag->n; i++)
	{
		for(int k=0; k < grid->m; k++)
		{
			IloExpr expr_restricao8(env);
			for(int t=0; t < tMax; t++)
			{
				expr_restricao += X[i][t][k];
			}
			model.add(F[i][k] = expr_restricao);
		}
	}

	//Restricao 9
	// TODO verificar dependências DAG e enlaces
	for(int i=0; i < dag->n; i++)
	{
		for(int k=0; k < dag->n; k++)
		{
			for(int l=0; l < grid->m; l++)
			{
				IloExpr expr_restricao9(env);
				expr_restricao9 = F[i][k] + F[k][l];
				model.add(2 * Y[i][k][k][l] <= expr_restricao);
			}
		}
	}

	// Restricao 10
	// TODO verificar dependências DAG e enlaces (mesmo R9)
	for(int i=0; i < *NUMERO_DE_TAREFAS*; i++)
	{
		for(int k=0; k < *NUMERO_DE_TAREFAS*; k++)
		{
			for(int l=0; l < *NUMERO_DE_HOSTS*; l++)
			{
				IloExpr expr_restricao10(env);
				expr_restricao10 = F[i][k] + F[k][l] - 1;
				model.add(Y[i][k][k][l] >= expr_restricao10);
			}
		}
	}

	// Restrições 11, 12 e 13 são de domínio, não precisam ser
	// codificadas como anteriores, já que X, U e Y já estão
	// declaradas como variáveis binárias



	//
	// Resolver
	//

	IloCplex cplex(model);

	/** Esta seção é para modificar o problema para que execute de forma mais rapida (implementação de heuristicas) **/
	try {
		if (algoritmo == "root_cplex-mip2"){ //Root Node Heuristic: Procura soluções alcansáveis na raiz da árvore da busca. 
			cplex.setParam(IloCplex::NodeLim, 1);
		}
		else {
			if (algoritmo == "time_cplex-mip2"){ //Time Heuristic: Limita o tempo de busca
				cplex.setParam(IloCplex::TiLim, tempo_limite);
			}
			else {
				if (algoritmo == "root_net_cplex-mip2"){ //IloCplex::Network optimiza o CPLEX para problemas orientados a redes de computadores 
					cplex.setParam(IloCplex::NodeLim, 1);
					cplex.setParam(IloCplex::RootAlg, IloCplex::Network);
				}
				else {
					if (algoritmo == "net_cplex-mip2"){
						cplex.setParam(IloCplex::RootAlg, IloCplex::Network);
					}
				}
			}
		}
		cplex.setParam(IloCplex::WorkMem, 512);
		cplex.setParam(IloCplex::WorkDir, "./");
		cplex.setParam(IloCplex::MemoryEmphasis, 1); // Ativo
		cplex.setParam(IloCplex::Threads, cores);   // "Cores" quantidade de nucleos a usar. Olhar restrições de uso das simuladoras (n/2 por usuário) 
		cplex.setParam(IloCplex::ParallelMode, 0);   // Automatico
		cplex.solve();
	} catch (IloCplex::Exception e) {
		cout << "ERRO (GetStatus): " << e.getStatus() << endl;
		cout << "ERRO (GetMessage): " << e.getMessage() << endl;
		if(cplex.getCplexStatus() == CPX_STAT_INFEASIBLE){
			mapeou = 0;
		}
		else {
			if(cplex.getCplexStatus() == CPXMIP_NODE_LIM_INFEAS){
				mapeou = -1;
			}
		}
	}

	// TODO daqui para baixo :P

	//
	// Atualizar variáveis de saída
	//
	try {
		for (int i = 0; i < parametros->num_nf; i++) {
			for (int j = 0; j < parametros->num_nv; j++) {
				for (int k = 0; k < parametros->num_ni; k++) {
					saida_mapnet->X[i][j][k] = cplex.getValue(X[i][j][k]);
				}
			}
		}

		for (int i = 0; i < parametros->num_nf; i++) {
			for (int j = 0; j < parametros->num_nf; j++) {
				for (int k = 0; k < parametros->num_ev; k++) {
					saida_mapnet->Y[i][j][k] = cplex.getValue(Y[i][j][k]);
				}
			}
		}
	} catch (IloCplex::Exception e) {
		cout << "ERRO (GetStatus): " << e.getStatus() << endl;
		cout << "ERRO (GetMessage): " << e.getMessage() << endl;
		if(cplex.getCplexStatus() == CPX_STAT_INFEASIBLE){
			mapeou = 0;
		}
		else {
			if(cplex.getCplexStatus() == CPXMIP_NODE_LIM_INFEAS){
				mapeou = -1;
			}
		}
	}

	//
	// Saída do CPLEX
	//

	try {
		cout << "Final (cplex.getCplexStatus = " << cplex.getCplexStatus() << endl;
		cout << "Status: " << cplex.getCplexStatus() << endl;
		cout << "Final (cplex.BestObjValue) = " << cplex.getObjValue() << endl;
	} 
	catch (IloCplex::Exception e) {
		cout << "ERRO (GetStatus): " << e.getStatus() << endl;
		cout << "ERRO (GetMessage): " << e.getMessage() << endl;
		if(cplex.getCplexStatus() == CPX_STAT_INFEASIBLE){
			mapeou = 0;
		}
		else {
			if(cplex.getCplexStatus() == CPXMIP_NODE_LIM_INFEAS){
			mapeou = -1;
			}
		}
	}

	//
	// Finalizando
	//

	env.end();
	return mapeou;

}
