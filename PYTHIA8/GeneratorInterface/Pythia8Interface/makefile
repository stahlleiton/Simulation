################################################################################
# VARIABLES: Definition of the relevant variables from the configuration script.
################################################################################

# Set the shell.
SHELL=/usr/bin/env bash

# Compilation flags (see ./configure --help for further documentation).
CXX=g++
CXX_COMMON=-O2 -std=c++11 -pedantic -W -Wall -Wshadow -fPIC
CXX_SHARED=-shared
CXX_SONAME=-Wl,-soname,
LIB_SUFFIX=.so

CXX_COMMON:=-I${ZLIB}/include -I${HEPMC3}/include -I${PHOTOS}/include -I${PYTHIA8}/include -I${EVTGEN}/include $(CXX_COMMON)
CXX_COMMON+= -L${HEPMC3LIB} -L${PHOTOS}/lib -L${PYTHIA8}/lib -L${EVTGENLIB}
CXX_COMMON+= -Wl,-rpath,${HEPMC3LIB},-rpath,${PHOTOS}/lib,-rpath,${PYTHIA8}/lib,-rpath,${EVTGENLIB}
CXX_COMMON+= -DGZIP -lHepMC3 -lPhotospp -lPhotosppHepMC3 -lpythia8 -ldl -lEvtGen -lEvtGenExternal

# Set the source and output
SRC := src
OBJ := bin
SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%, $(SOURCES))

################################################################################
# RULES: Definition of the rules used to build
################################################################################

# All targets
all: $(OBJECTS)

$(OBJ)/%: $(SRC)/%.cpp | $(OBJ)
	$(CXX) -o $@ $< -w $(CXX_COMMON) `root-config --cflags --glibs`

$(OBJ):
	mkdir -p $@

# PYTHIA library.
${PYTHIA8}/lib/libpythia8$(LIB_SUFFIX):
	$(error Error: PYTHIA must be built, please run "make" in the top PYTHIA directory)

clean:
	rm $(OBJ)/*
