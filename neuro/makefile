CPP = g++
MPICC = mpic++
BIN = bin
INC = include
OBJ = obj
SRC = src
BASEDIR = ../../neuro
NINC1 = $(BASEDIR)/include
NINC2 = $(BASEDIR)/include/utils

# Can be changed to nida or another neuromorphic model as needed.
# I.e. make model=nida

model = nida
NLIB = $(BASEDIR)/models/$(model)/lib/libneuro.a
NEO = $(BASEDIR)/models/$(model)/eo/lib/libneuroEO.a

EO = $(BASEDIR)/eo

EO_OBJ = $(EO)/gengraph/obj/main.o $(EO)/gengraph/lib/libEOGG.a
DEO_OBJ = $(EO)/gengraph/obj/main-deo.o $(EO)/gengraph/lib/libEOGG.a $(EO)/gengraph/lib/NeuroDEO.a

EO_LEGACY_OBJ = $(EO)/legacy/obj/main.o $(EO)/legacy/obj/NeuroEO.o

CFLAGSBASE = -O3 -Wall -Wextra -I$(INC) -I$(NINC1) -I$(NINC2) -pthread
CFLAGSNEURO = -I$(NINC1) -I$(NINC2)

LFLAGS = $(BASEDIR)/lib/libneuro.a $(NLIB)

all: $(BIN)/TWAP
#all: $(BIN)/TestAE $(BIN)/TWAP $(BIN)/TestNL $(BIN)/XorNP $(BIN)/XorEOLegacy $(BIN)/XorEOGG \
     $(BIN)/XorTestFitness

eogg: $(BIN)/XorEOGG

clean:
	rm -f $(OBJ)/* $(BIN)/TestAE $(BIN)/TWAP $(BIN)/TestNL $(BIN)/XorNP $(BIN)/XorEOGG  \
		$(BIN)/XorTestFitness $(BIN)/XorEOLegacy

$(BIN)/XorTestFitness: scripts/XorTestFitness.sh
	cp scripts/XorTestFitness.sh $(BIN)/XorTestFitness
	chmod 0755 $(BIN)/XorTestFitness

$(BIN)/TestAE: $(SRC)/TestAE.cpp $(OBJ)/TWAE.o $(INC)/TWAE.hpp
	$(CPP) $(CFLAGSBASE) $(SRC)/TestAE.cpp $(OBJ)/TWAE.o -o $(BIN)/TestAE

$(BIN)/TWAP: $(SRC)/TWAP.cpp $(OBJ)/TWAE.o $(INC)/TWAE.hpp
	$(CPP) $(CFLAGSBASE) $(SRC)/TWAP.cpp $(OBJ)/TWAE.o $(BASEDIR)/lib/libneuro.a -o $(BIN)/TWAP

$(BIN)/XorNP: $(SRC)/XorNP.cpp $(OBJ)/XorNLib.o $(OBJ)/TWAE.o $(NLIB) $(BASEDIR)/lib/libneuro.a
	$(CPP) $(CFLAGSBASE) \
               $(CFLAGSNEURO) \
               $(SRC)/XorNP.cpp $(OBJ)/XorNLib.o $(OBJ)/TWAE.o -o $(BIN)/XorNP \
               $(LFLAGS)

$(BIN)/TestBatch: $(SRC)/TestBatch.cpp $(OBJ)/XorNLib.o $(NLIB) $(BASEDIR)/lib/libneuro.a
	$(CPP) $(CFLAGSBASE) $(CFLAGSNEURO) $(SRC)/TestBatch.cpp $(OBJ)/XorNLib.o -o $(BIN)/TestBatch $(LFLAGS)

$(BIN)/TestNL: $(SRC)/TestNL.cpp $(OBJ)/XorNLib.o $(OBJ)/TWAE.o ../../models/dummy/lib/libneuro.a $(BASEDIR)/lib/libneuro.a
	$(CPP) $(CFLAGSBASE) \
            $(CFLAGSNEURO) \
	    $(SRC)/TestNL.cpp $(OBJ)/XorNLib.o $(OBJ)/TWAE.o -o $(BIN)/TestNL \
	    $(BASEDIR)/lib/libneuro.a ../../models/dummy/lib/libneuro.a

$(OBJ)/TWAE.o: $(SRC)/TWAE.cpp $(INC)/TWAE.hpp
	$(CPP) $(CFLAGSBASE) $(SRC)/TWAE.cpp -c -o $(OBJ)/TWAE.o

$(OBJ)/XorNLib.o: $(SRC)/XorNLib.cpp $(INC)/XorNLib.hpp
	$(CPP) $(CFLAGSBASE) $(CFLAGSNEURO) $(SRC)/XorNLib.cpp -c -o $(OBJ)/XorNLib.o

$(OBJ)/XorEO.o: $(SRC)/XorEO.cpp $(INC)/TWAE.hpp $(INC)/XorNLib.hpp
	$(CPP) $(CFLAGSBASE) $(CFLAGSNEURO) $(SRC)/XorEO.cpp -c -o $(OBJ)/XorEO.o

$(BIN)/XorEOLegacy: $(OBJ)/XorEO.o $(OBJ)/TWAE.o $(OBJ)/XorNLib.o $(NLIB) $(NEO) $(EO_LEGACY_OBJ) $(BASEDIR)/lib/libneuro.a
	$(CPP) $(CFLAGSBASE) \
            $(CFLAGSNEURO) \
            $(OBJ)/XorEO.o $(OBJ)/TWAE.o $(OBJ)/XorNLib.o -o $(BIN)/XorEOLegacy \
            $(EO_LEGACY_OBJ) \
            $(LFLAGS) \
            $(NEO)

$(BIN)/XorEOGG: $(OBJ)/XorEO.o $(OBJ)/TWAE.o $(OBJ)/XorNLib.o $(NLIB) $(NEO) $(EO_OBJ) $(BASEDIR)/lib/libneuro.a
	$(CPP) $(CFLAGSBASE) \
            $(CFLAGSNEURO) \
            $(OBJ)/XorEO.o $(OBJ)/TWAE.o $(OBJ)/XorNLib.o -o $(BIN)/XorEOGG \
            $(EO_OBJ) \
            $(LFLAGS) \
            $(NEO)

$(BIN)/XorDEO: $(OBJ)/XorEO.o $(OBJ)/TWAE.o $(OBJ)/XorNLib.o $(NLIB) $(NEO) $(DEO_OBJ) $(BASEDIR)/lib/libneuro.a
	$(MPICC) $(CFLAGSBASE) $(CFLAGSNEURO) $(OBJ)/XorEO.o $(OBJ)/TWAE.o $(OBJ)/XorNLib.o -o $(BIN)/XorDEO $(DEO_OBJ) $(LFLAGS) $(NEO)

