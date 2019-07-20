#------------------------------------------------------------
#
# Please define an environnement variable PATHTUTOMIP
# equal to the XXX directory where Lemon,Graphviz and Cplex have been installed
# In order to these:
# edit ~/.bashrc add "export PATHTUTOMIP=XXX"
# close your xterm and star new ones
#
#------------------------------------------------------------

CPLEXDIR      = $(PATHTUTOMIP)/cplex-12.7.1/cplex
CONCERTDIR    = $(PATHTUTOMIP)/cplex-12.7.1/concert
LEMONDIR      = $(PATHTUTOMIP)/lemon-1.3.1


SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic


# ---------------------------------------------------------------------
# Compiler selection
# ---------------------------------------------------------------------

CCC = g++ -O0


# ---------------------------------------------------------------------
# Compiler options
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wno-ignored-attributes

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
LEMONLIBDIR	= $(LEMONDIR)/lib

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) -L$(LEMONLIBDIR)


CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
LEMONINCDIR   = $(LEMONDIR)/include

CCLNFLAGS = -lconcert -lilocplex -lcplex -lemon -lm -lpthread

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(LEMONINCDIR)


# ---------------------------------------------------------------------
# Project
# ---------------------------------------------------------------------

all: executable

main.o : main.cpp Data.h PLNE_vrp.h DetecteCycle.h DetecteCycle_directed.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c main.cpp -o main.o

Data.o : Data.cpp Data.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c Data.cpp -o Data.o

PLNE_vrp.o : PLNE_vrp.cpp PLNE_vrp.h Graph_lemon.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c PLNE_vrp.cpp -o PLNE_vrp.o

Graph_lemon.o : Graph_lemon.cpp Graph_lemon.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c Graph_lemon.cpp -o Graph_lemon.o

Graph_lemon_directed.o : Graph_lemon_directed.cpp Graph_lemon_directed.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c Graph_lemon_directed.cpp -o Graph_lemon_directed.o

DetecteCycle.o : DetecteCycle.cpp DetecteCycle.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c DetecteCycle.cpp -o DetecteCycle.o

DetecteCycle_directed.o : DetecteCycle_directed.cpp DetecteCycle_directed.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c DetecteCycle_directed.cpp -o DetecteCycle_directed.o

executable : main.o Data.o  PLNE_vrp.o Graph_lemon.o Graph_lemon_directed.o DetecteCycle.o DetecteCycle_directed.o
		$(CCC) $(CCFLAGS) $(CCLNDIRS) main.o Data.o  PLNE_vrp.o Graph_lemon.o Graph_lemon_directed.o DetecteCycle.o DetecteCycle_directed.o $(CCLNFLAGS) -o executable

clean:
	rm -f *.o executable
