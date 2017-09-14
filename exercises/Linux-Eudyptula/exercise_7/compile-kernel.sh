#!/bin/sh

# Script to compile and install kernel

if [ -z $1 ]; then
	echo "You must enter LOCAL or DISTANT to choose correct kernel source"
	exit -1
fi

LOCAL_OR_DISTANT=$1
VERSION="next"
NB_PROC=$(cat /proc/cpuinfo | grep processor | wc -l)
DIR=/usr/localc/src
REPO="/home/Renault/Programmation/linux"

if [ "$LOCAL_OR_DISTANT" == "DISTANT" ]; then
	if [ "$VERSION" == "next" ]; then
		cd $DIR
		git clone git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git
	else
		cd $DIR
		wget http://www.kernel.org/pub/linux/kernel/v4.x/linux-$VERSION.tar.xz
		tar -xvf linux-$VERSION.tar.xz
		rm -f linux-$VERSION.tar.xz
	fi

	cd linux-$VERSION/
else
	cd $REPO
fi

cp /boot/config-$(uname -r) .config

make -j$NB_PROC
make modules
make modules_install
make install

cp .config /boot/config-$VERSION
