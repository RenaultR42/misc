#!/bin/sh

# Script to comoile specific GCC version, needed sometimes for fixed components
# (like CUDA librairies)

VERSION=4.8.5
NB_PROC=$(cat /proc/cpuinfo | grep processor | wc -l)

wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-$VERSION/gcc-$VERSION.tar.bz2
tar xf gcc-$VERSION.tar.bz2
rm gcc-$VERSION.tar.bz2

cd gcc-$VERSION
./contrib/download_prerequisites

mkdir objdir
cd objdir
../configure --prefix=$(pwd)/../../gcc-$VERSION-x86_64 --enable-languages=c,c++ --disable-multilib

make -j$NB_PROC
make install
