#!/bin/sh

# Script to flash Tegra K1 Jetson with our buildroot solution.

OPTIONS="$1"

PREFIX="/home/Renault/Documents/Antares/code/sw"
PREFIX_UBOOT_NTK="$PREFIX/asp/buildroot/boot/uboot_ntk/"
PREFIX_OUTPUT="$PREFIX/buildroot/output"

IP_SERVER=$(cat $PREFIX/buildroot/.config | grep BR2_NXV_NFS_SERVER_IP | cut -d '=' -f 2 | cut -d '"' -f 2)
IP_CLIENT=$(cat $PREFIX/buildroot/.config | grep BR2_NXV_NFS_CLIENT_IP | cut -d '=' -f 2 | cut -d '"' -f 2)
BOARD=$(cat $PREFIX/buildroot/.config | grep BR2_TARGET_UBOOT_NTK_BOARD_DEFCONFIG | cut -d '=' -f 2 | cut -d '"' -f 2)

if [ "$OPTIONS" == "FLASH" -o "$OPTIONS" == "ALL" ]; then
	$PREFIX_UBOOT_NTK/generate-flash-nfs.sh $PREFIX_UBOOT_NTK/flash-nfs.sh $PREFIX_OUTPUT/host $BOARD $IP_SERVER $IP_CLIENT $PREFIX_OUTPUT/nfs $PREFIX_OUTPUT/images

	chmod +x $PREFIX_UBOOT_NTK/flash-nfs.sh
	$PREFIX_UBOOT_NTK/flash-nfs.sh
fi

if [ "$OPTIONS" == "EXTRACT" -o "$OPTIONS" == "ALL" ]; then
	systemctl start nfs
	$PREFIX/asp/buildroot/board/nexvision/export-nfs.sh $PREFIX_OUTPUT/images $PREFIX_OUTPUT/nfs
fi

if [ "$OPTIONS" == "EXEC" ]; then
	export PATH="$PREFIX_OUTPUT/host/usr/bin:$PATH"
	$PREFIX_OUTPUT/host/usr/bin/tegra-uboot-flasher --debug --force-no-out-dir --data-dir $PREFIX_OUTPUT/host/usr/share/tegra-uboot-flasher exec --bootloader $PREFIX_OUTPUT/build/uboot_ntk-asp/u-boot --loadaddr 0x80108000 ntk
fi
