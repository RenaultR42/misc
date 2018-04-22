#!/bin/sh

# Script to copy correct u-boot / system image in SD card for one project based on Yocto solution.
# Execute with root permissions
MODE=$1

if [ -z "$MODE" ]; then
    echo "No argument! Select COPY, SD"
    exit
fi 

SD="/mnt"
REPO="/home/Renault/Documents/Mind/cmc/io-monitor/build_f/tmp/deploy/images/cmc-imx6ul/"

if [ "$MODE" == "SD" ]; then
    dd if=$REPO/cmc-imx6ul-image-cmc-imx6ul.sdcard of=/dev/mmcblk0
else
    mount /dev/mmcblk0p3 $SD

    if [ $? == 0 ]; then
        cp $REPO/SPL $SD/
        cp $REPO/u-boot.img $SD/
        cp $REPO/zImage $SD/
        cp $REPO/zImage-imx6ull-var-dart-sd_emmc.dtb $SD/
        cp $REPO/cmc-imx6ul-image-cmc-imx6ul.ext4 $SD/rootfs.ext4
        cp $REPO/cmc-imx6ul-image-cmc-imx6ul.tar.gz $SD/rootfs.tar.gz

        umount /mnt
    else
       echo "Error to mount /dev/mmcblk0p3"
    fi
fi

sync
