/*
   CPLEX.cpp
*/

#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include "ilcplex/ilocplex.h"
#include "ilconcert/iloxmlcontext.h"
#include "mip-v3.h"
#include "mip.h"

using namespace std;

//
// Integer Programming formulation 
//

/** Os parámetros desta função serão as referencias das estruturas de datos que contém a informação da simulação. Para este exemplo, suponha
que parametros é um punteiro a uma estrutura que contém toda a informação que precisa esta modelagem em particular. 
 
    Também é possível parametrizar outras coisas, como bandeiras para determinar quais restrições a ser usadas ou parametros para determinar se alguma heuristicas vai ser usada. **/

int ILP(Param_mapnet * parametros, string algoritmo) {

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

   //
   // Definição das variaveis
   // Suponha uma variavel X de três dimenções, e uma variavel Y de três dimenções

   IloBoolVarArray3 X(env, *NUMERO_DE_TAREFAS*);
   for (int i = 0; i < *NUMERO_DE_TAREFAS*; i++) {
      X[i]  = IloBoolVarArray2(env, *TEMPOS_DE_TERMINO*);
      for (int j = 0; j < *TEMPOS_DE_TERMINO*; j++) {
         X[i][j]  = IloBoolVarArray(env, *NUMERO_DE_HOSTS*);
      }
   }
   
   
   IloBoolVarArray4 Y(env, *NUMERO_DE_TAREFAS*);
   for (int i = 0; i < *NUMERO_DE_TAREFAS*; i++) {
      Y[i] = IloBoolVarArray3(env, *NUMERO_DE_TAREFAS*);
      for(int j = 0; j < *NUMERO_DE_TAREFAS*, j++)
      {
        Y[i][j] = IloBoolVarArray2(env, *NUMERO_DE_HOSTS*);
        for(int k = 0; k < *NUMERO_DE_HOSTS*; k++)
        {
            Y[i][j][k] = IloBoolVarArray(env, *NUMERO_DE_HOSTS*);
        }
      }
   }
   
   IloBoolVarArray2 U(env, *NUMERO_DE_HOSTS*);
   for(int i=0; i < *NUMERO_DE_HOSTS*, i++)
   {
        U[i] = IloBoolVarArray(env, *TEMPOS_DE_TERMINO*);
   }

   IloBoolVarArray2 P(env, *NUMERO_DE_HOSTS*);
   for(int i=0; i < *NUMERO_DE_HOSTS*, i++)
   {
        P[i] = IloBoolVarArray(env, *TEMPOS_DE_TERMINO*);
   }

   IloBoolVarArray2 F(env, *NUMERO_DE_TAREFAS*);
   for(int i=0; i < *NUMERO_DE_HOSTS*, i++)
   {
        F[i] = IloBoolVarArray(env, *NUMERO_DE_HOSTS*);
   }

   /* Função Objetivo */
   IloExpr expr_objetivo(env);

   for(int u = 0; u < parametros->num_nf; u++) {
      for(int v = 0; v < parametros->num_nf; v++) {
         for(int w = 0; w < parametros->num_ev; w++) {
            expr_objetivo += Y[u][v][w] * parametros->c_ev_b[w];
         }
      }
   }

   IloObjective obj = IloMinimize(env, expr_objetivo);
   model.add(obj);

   /************************************************
                       RESTRICOES
   *************************************************/

   //
   // Restrição 1 - Restrição para garantir que para um nó virtual será alocado em exatamente
   // um nó físico e irá utilizar exatamente uma imagem virtual
   //
   for(int m = 0; m < parametros->num_nv; m++) {
      IloExpr expr_restricao1(env);
      for(int n = 0; n < parametros->num_nf; n++) {
         for(int i = 0; i < parametros->num_ni; i++) {
            expr_restricao1 += X[n][m][i];
         }
      }
      model.add(expr_restricao1 == 1);
   }

   //
   // Resolver
   //

   IloCplex cplex(model);

   /** Esta seção é para modificar o problema para que execute de forma mais rapida (implementação de heuristicas) **/
   try {
      if (algoritmo == "root_cplex-mip2"){ //Root Node Heuristic: Procura soluções alcanzaveis na raiz da árvore da busca. 
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

