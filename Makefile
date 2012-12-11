SYSTEM64     = x86-64_sles10_4.1
SYSTEM     = x86_sles10_4.1
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /home/rodrigo/ILOG/CPLEX_Studio125/cplex
CONCERTDIR    = /home/rodrigo/ILOG/CPLEX_Studio125/concert
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++-4.4
#CCC = g++-4.5

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT64 = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD
CCOPT = -O -fPIC -fexceptions -DNDEBUG -DIL_STD

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CPLEXLIBDIR64   = $(CPLEXDIR)/lib/$(SYSTEM64)/$(LIBFORMAT)

CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR64 = $(CONCERTDIR)/lib/$(SYSTEM64)/$(LIBFORMAT)

CCLNFLAGS64 = -L$(CPLEXLIBDIR64) -lilocplex -lcplex -L$(CONCERTLIBDIR64) -lconcert -m64 -lm -pthread
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include


CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 
CCFLAGS64 = $(CCOPT64) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples. (TODO)
#------------------------------------------------------------
dag:
	$(CCC) -c $(CCFLAGS) dag.cpp -o dag.o

grid:
	$(CCC) -c $(CCFLAGS) grid.cpp -o grid.o

ilp: dag grid
	$(CCC) -c $(CCFLAGS) ilp.cpp -o ilp.o

simulador: dag grid ilp
	$(CCC) -c $(CCFLAGS) simulador.cpp -o simulador.o

all: dag grid ilp simulador
	$(CCC) $(CCFLAGS) dag.o grid.o ilp.o simulador.o -o simulador $(CCLNFLAGS)

all-64: dag grid ilp simulador
	$(CCC) $(CCFLAGS64) dag.o grid.o ilp.o simulador.o -o simulador-64 $(CCLNFLAGS64)

execute: all
	./simulador NOVA_SEMENTE

# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o 

