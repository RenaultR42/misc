#!/bin/sh

# Script to copy correct u-boot / system image in SD card for one project based on ptxdist solution.
# Execute with root permissions
MODE=$1

if [ -z "$MODE" ]; then
	echo "No argument! Select AUTO, SD, EEPROM or NOR mode"
	exit
fi 

SD="/mnt"
REPO="/home/Renault/Documents/CVMS/code"

UBOOT="$REPO/rootfs/platform-dau/images"
UBOOT_NOR="$REPO/repo/cvms-dau-boot-develop/rootfs/platform-dau/images"
UBOOT_AUTO_NOR="$REPO/repo/cvms-dau-boot-master/rootfs/platform-dau/images"
UBOOT_AUTO="$REPO/repo/cvms-dau-boot-sd-master/rootfs/platform-dau/images"

IMAGES="$REPO/repo/cvms-dau-opsw-develop/rootfs/platform-dau/images"
IMAGES_NOR="$REPO/repo/cvms-dau-boot-master/rootfs/platform-dau/images"
IMAGES_DLO="$REPO/repo/cvms-dau-opsw-develop/images"

mount /dev/mmcblk0p1 $SD

if [ "$MODE" == "SD" ]; then
    mkimage -A arm -O linux -T ramdisk -d $IMAGES/root.cpio.gz $IMAGES/root.Image
    cp $UBOOT/MLO $SD/
    cp $UBOOT/u-boot.img $SD/
    cp $IMAGES/linuximage $SD/
    cp $IMAGES/root.Image $SD/
    cp $IMAGES/root.squashfs $SD/
elif [ "$MODE" == "AUTO" ]; then
    mkdir -p $SD/bootdau
    mkdir -p $SD/dau
    cp $UBOOT_AUTO/MLO $SD/
    cp $UBOOT_AUTO/u-boot.img $SD/
    cp $UBOOT_AUTO_NOR/u-boot.bin $SD/bootdau/
    cp $IMAGES_DLO/dau-opsw-kernel $SD/bootdau/linuximage
    cp $IMAGES_DLO/dau-opsw-dlo $SD/dau/
elif [ "$MODE" == "EEPROM" ]; then
    cp $UBOOT/MLO $SD/
    cp $UBOOT/u-boot.img $SD/
    cp $UBOOT_AUTO_NOR/u-boot.bin $SD/
    cp $IMAGES_NOR/linuximage $SD/
else
    echo "To flash NOR devices, do that on u-boot shell:"
    echo "protect off 0x10000000 0x1009FFFF"
    echo "erase 0x10000000 0x11BFFFFF"
    echo "fatload mmc 0 0x82000000 u-boot.bin"
    echo "cp.b 0x82000000 0x10000000 \${filesize}"
    echo "fatload mmc 0 0x83000000 linuximage"
    echo "cp.b 0x83000000 0x10080000 \${filesize}"
    echo "fatload mmc 0 0x84000000 root.squashfs"
    echo "cp.b 0x84000000 0x10C00000 \${filesize}"

    cp $UBOOT_AUTO_NOR/u-boot.bin $SD/
    cp $IMAGES/linuximage $SD/
    cp $IMAGES/root.squashfs $SD/
fi

sync
umount /dev/mmcblk0p1
