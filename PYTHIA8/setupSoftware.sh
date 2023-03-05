#!/bin/bash

# Prerequisites:
# Linux (ubuntu): you need to install some libraries before attempting trying to install the generator packages.
# This can be done by running:
# sudo apt-get install -y dpkg-dev manpages-dev build-essential cmake g++ gcc binutils curl git python3-dev python3-apt python3-distutils libx11-dev libxpm-dev libxft-dev libxext-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev gfortran libssl-dev libpcre3-dev libftgl-dev libmysqlclient-dev libfftw3-dev libcfitsio-dev libavahi-compat-libdnssd-dev libldap2-dev libxml2-dev libkrb5-dev qtcreator qt5-default
# Linux (CentOS):
# sudo yum install -y git curl cmake3 gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel openssl-devel gcc-gfortran pcre-devel mesa-libGL-devel mesa-libGLU-devel glew-devel ftgl-devel mysql-devel fftw-devel cfitsio-devel graphviz-devel avahi-compat-libdns_sd-devel openldap-devel python3-devel libxml2-devel gsl-devel

# Number of cores used for build and install
NCORES=4
# Location in which to install
INSTALL_PREFIX="${PWD}/Software"

# EvtGen version or tag number (or branch name). No extra spaces on this line!
EVTGENVER=132d819899e9 #R02-00-00 with bug fix for missing header file
EVTGENPKG="EvtGen-02.00.00"

# HepMC version number
HEPMCVER="3.2.3"
HEPMCPKG="HepMC3-"$HEPMCVER
HEPMCTAR=$HEPMCPKG".tar.gz"

# Pythia version number with no decimal points, e.g. 8230 corresponds to version 8.230. This
# follows the naming convention of Pythia install tar files. Again, no extra spaces allowed.
PYTHIAVER=8303
PYTHIAPKG="pythia"$PYTHIAVER
PYTHIATAR=$PYTHIAPKG".tgz"

# Photos++ version number
PHOTOSVER="3.64"
PHOTOSPKG="PHOTOS"
PHOTOSDIR="PHOTOS."$PHOTOSVER
PHOTOSTAR=$PHOTOSDIR".tar.gz"

# LHAPDF6 version number
LHAPDFVER="6.3.0"
LHAPDFPKG="LHAPDF-"$LHAPDFVER
LHAPDFTAR=$LHAPDFPKG".tar.gz"

# ZLIB version number
ZLIBVER="1.2.11"
ZLIBPKG="zlib-"$ZLIBVER
ZLIBTAR=$ZLIBPKG".tar.gz"

# ROOT version number
ROOTVER="6.22.08"
ROOTPKG="root-"$ROOTVER
ROOTTAR="root_v"$ROOTVER".source.tar.gz"


#Use LCG installation settings on LXPLUS
if [ -d "/cvmfs/sft.cern.ch/lcg" ]; then
    source /cvmfs/sft.cern.ch/lcg/releases/gcc/8.3.0/x86_64-centos7/setup.sh
fi

# Determine OS
osArch=`uname`

#This is for systems with cmake and cmake3
if command -v cmake3; then
    CMAKE=cmake3
else
    CMAKE=cmake
fi

if [ "$osArch" == "Darwin" ]
then
    export LANG=en_US.UTF-8
    export LC_ALL=en_US.UTF-8
fi

BUILD_BASE=`mktemp -d` || exit 1

echo Temporary build area is $BUILD_BASE
cd $BUILD_BASE
mkdir -p tarfiles
mkdir -p sources

echo Downloading tar archive packages
cd $BUILD_BASE/tarfiles
curl https://lhapdf.hepforge.org/downloads/?f=$LHAPDFTAR -o $LHAPDFTAR
curl http://hepmc.web.cern.ch/hepmc/releases/$HEPMCTAR -o $HEPMCTAR
curl http://home.thep.lu.se/~torbjorn/pythia8/$PYTHIATAR -o $PYTHIATAR
curl http://photospp.web.cern.ch/photospp/resources/$PHOTOSDIR/$PHOTOSTAR -o $PHOTOSTAR
curl https://zlib.net/fossils/$ZLIBTAR -o $ZLIBTAR
curl https://root.cern/download/$ROOTTAR -o $ROOTTAR

echo Extracting tar archive packages
cd $BUILD_BASE/sources
tar -xzf $BUILD_BASE/tarfiles/$LHAPDFTAR
tar -xzf $BUILD_BASE/tarfiles/$HEPMCTAR
tar -xzf $BUILD_BASE/tarfiles/$PYTHIATAR
tar -xzf $BUILD_BASE/tarfiles/$PHOTOSTAR
tar -xzf $BUILD_BASE/tarfiles/$ZLIBTAR
tar -xzf $BUILD_BASE/tarfiles/$ROOTTAR
rm -rf $BUILD_BASE/tarfiles

echo Downloading git packages
cd $BUILD_BASE/sources
git clone https://phab.hepforge.org/source/evtgen.git $EVTGENPKG
cd $EVTGENPKG; git checkout $EVTGENVER; cd -

# Patch PHOTOS on Darwin (Mac)
if [ "$osArch" == "Darwin" ]
then
    sed -i '' 's/soname/install_name/g' PHOTOS/Makefile
    patch -p0 < $BUILD_BASE/sources/$EVTGENPKG/platform/photos_Darwin.patch
fi


echo Installing ZLIB from $BUILD_BASE/sources/$ZLIBPKG
cd $BUILD_BASE/sources/$ZLIBPKG
./configure --prefix=$INSTALL_PREFIX/$ZLIBPKG
make -j$NCORES
make install -j$NCORES

echo Installing LHAPDF6
cd $BUILD_BASE/sources/$LHAPDFPKG
./configure --prefix=$INSTALL_PREFIX/$LHAPDFPKG
make -j$NCORES
make install -j$NCORES

echo Downloading LHAPDF sets
for PDF in CT09MCS MSTW2008nnlo90cl cteq6l1; do
    wget -q http://lhapdfsets.web.cern.ch/lhapdfsets/current/${PDF}.tar.gz -O- | tar xz -C $INSTALL_PREFIX/$LHAPDFPKG/share/LHAPDF
done

echo Installing HepMC3 from $BUILD_BASE/sources/$HEPMCPKG
mkdir -p $BUILD_BASE/builds/$HEPMCPKG
cd $BUILD_BASE/builds/$HEPMCPKG
$CMAKE -DHEPMC3_ENABLE_ROOTIO:BOOL=OFF -DHEPMC3_ENABLE_TEST:BOOL=OFF -DHEPMC3_INSTALL_INTERFACES:BOOL=ON -DHEPMC3_ENABLE_PYTHON:BOOL=OFF -DHEPMC3_BUILD_STATIC_LIBS:BOOL=OFF -DHEPMC3_BUILD_DOCS:BOOL=OFF -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX/$HEPMCPKG $BUILD_BASE/sources/$HEPMCPKG
make -j$NCORES
make install -j$NCORES
HEPMCLIB=$INSTALL_PREFIX/$HEPMCPKG/$([[ -d "$INSTALL_PREFIX/$HEPMCPKG/lib64" ]] && echo "lib64" || echo "lib")

echo Installing pythia8 from $BUILD_BASE/sources/$PYTHIAPKG
cd $BUILD_BASE/sources/$PYTHIAPKG
./configure --cxx=g++ --prefix=$INSTALL_PREFIX/$PYTHIAPKG --with-lhapdf6=$INSTALL_PREFIX/$LHAPDFPKG --with-lhapdf6-include=$INSTALL_PREFIX/$LHAPDFPKG/include --with-gzip=$INSTALL_PREFIX/$ZLIBPKG --with-hepmc3=$INSTALL_PREFIX/$HEPMCPKG --with-hepmc3-include=$INSTALL_PREFIX/$HEPMCPKG/include --with-hepmc3-lib=$HEPMCLIB
make -j$NCORES
make install -j$NCORES

echo Installing ROOT from $BUILD_BASE/sources/$ROOTPKG
mkdir -p $BUILD_BASE/builds/$ROOTPKG
cd $BUILD_BASE/builds/$ROOTPKG
$CMAKE -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX/$ROOTPKG $BUILD_BASE/sources/$ROOTPKG -DPYTHIA8_DIR=$INSTALL_PREFIX/$PYTHIAPKG -DPYTHIA8_INCLUDE_DIR=$INSTALL_PREFIX/$PYTHIAPKG/include -Dbuiltin_glew=ON -Doracle=OFF -Dxrootd=OFF
cmake --build . --target install -- -j$NCORES
source $INSTALL_PREFIX/$ROOTPKG/bin/thisroot.sh

echo Installing PHOTOS from $BUILD_BASE/sources/$PHOTOSPKG
cd $BUILD_BASE/sources/$PHOTOSPKG
./configure --prefix=$INSTALL_PREFIX/$PHOTOSDIR --without-hepmc --with-hepmc3=$INSTALL_PREFIX/$HEPMCPKG --with-pythia8=$INSTALL_PREFIX/$PYTHIAPKG
make -j$NCORES
make install -j$NCORES

echo Installing EvtGen from $BUILD_BASE/sources/$EVTGENPKG
mkdir -p $BUILD_BASE/builds/$EVTGENPKG
cd $BUILD_BASE/builds/$EVTGENPKG
$CMAKE -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX/$EVTGENPKG $BUILD_BASE/sources/$EVTGENPKG \
    -DEVTGEN_HEPMC3:BOOL=ON  -DHEPMC3_ROOT_DIR:PATH=$INSTALL_PREFIX/$HEPMCPKG \
    -DEVTGEN_PYTHIA:BOOL=ON  -DPYTHIA8_ROOT_DIR:PATH=$INSTALL_PREFIX/$PYTHIAPKG \
    -DEVTGEN_PHOTOS:BOOL=ON  -DPHOTOSPP_ROOT_DIR:PATH=$INSTALL_PREFIX/$PHOTOSDIR \
    -DEVTGEN_TAUOLA:BOOL=OFF
make -j$NCORES
make install -j$NCORES

echo Setup done.
echo To complete, set the Pythia8 path:
echo PYTHIA8=$INSTALL_PREFIX/$PYTHIAPKG
echo PYTHIA8DATA=$INSTALL_PREFIX/$PYTHIAPKG/share/Pythia8/xmldoc

echo If installation fully successful you can remove the temporary build area $BUILD_BASE
rm -rf $BUILD_BASE/sources
rm -rf $BUILD_BASE/builds
