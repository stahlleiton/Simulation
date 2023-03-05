#!/bin/bash

#Use LCG installation settings on LXPLUS
if [ -d "/cvmfs/sft.cern.ch/lcg" ]; then
    source /cvmfs/sft.cern.ch/lcg/releases/gcc/8.3.0/x86_64-centos7/setup.sh
fi

export ZLIB=${PWD}/Software/zlib-1.2.11

export HEPMC3=${PWD}/Software/HepMC3-3.2.3
export HEPMC3LIB=$HEPMC3/$([[ -d "$HEPMC3/lib64" ]] && echo "lib64" || echo "lib")

export PHOTOS=${PWD}/Software/PHOTOS.3.64

export EVTGEN=${PWD}/Software/EvtGen-02.00.00
export EVTGENLIB=$EVTGEN/$([[ -d "$EVTGEN/lib64" ]] && echo "lib64" || echo "lib")

export PYTHIA8=${PWD}/Software/pythia8303
export PYTHIA8DATA=${PYTHIA8}/share/Pythia8/xmldoc

source ${PWD}/Software/root-6.22.08/bin/thisroot.sh
