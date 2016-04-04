#!/bin/sh

# Script to compile upstream OpenSSL, usefull if you need some algorithms disable
# in Fedora for licences issues.

VERSION=1.0.2g
NB_PROC=$(cat /proc/cpuinfo | grep processor | wc -l)

wget https://www.openssl.org/source/openssl-$VERSION.tar.gz
tar -xf openssl-$VERSION.tar.gz
rm openssl-$VERSION.tar.gz

cd openssl-$VERSION
./config
make -j$NB_PROC
make install
