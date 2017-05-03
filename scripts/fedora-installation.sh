#!/bin/sh

# Script to complete installation of my Fedora

# Repos
RAWHIDE=$(cat /etc/fedora-release | grep Rawhide | wc -l)
RPMFUSION_URL="http://download1.rpmfusion.org/free/fedora"
NO_DEVEL="--exclude=*-devel"
SKIP_MISSING_PACKAGE="--setopt=strict=0"
LANGUAGE="fr"

## RPMFusion
if [ "$RAWHIDE" -eq 0 ]; then
	rpm -ivh $RPMFUSION_URL/rpmfusion-free-release-stable.noarch.rpm 
else
	rpm -ivh $RPMFUSION_URL/rpmfusion-free-release-rawhide.noarch.rpm 
fi

## Testing
if [ "$RAWHIDE" -eq 0 ]; then
	dnf config-manager --set-enabled fedora updates updates-testing
	dnf config-manager --set-enabled rpmfusion-free rpmfusion-free-updates rpmfusion-free-updates-testing
else
	dnf config-manager --set-enabled fedora-rawhide
	dnf config-manager --set-enabled rpmfusion-rawhide
fi

# Packages
## System update
dnf -y upgrade

## Base
dnf -y groupinstall "Fedora Workstation"
dnf -y install dnf-plugin-system-upgrade deja-dup vlc mplayer mplayer-gui mencoder thunderbird\* lame gstreamer\*-plugin*\ gstreamer1-libav gstreamer1-vaapi ffmpeg gpm xvidcore empathy transmission-gtk --exclude=\*docs $NO_DEVEL $SKIP_MISSING_PACKAGE

## GNOME
dnf -y groupinstall "Environnement de bureau standard"
dnf -y install gnome-tweak-tool gnome-weather gitg epiphany swell-foop hitori tali iagno lightsoff quadrapassel four-in-a-row gnome-tetravex gnome-nibbles gnome-taquin gnome-mahjongg gnome-robots gnome-sudoku five-or-more gnome-maps gnome-chess gnome-mines gnome-clocks gnome-boxes polari gnome-music gnome-photos ghex ekiga gnome-tweak-tool gnome-hearts simple-scan gnome-mime-data dconf-editor \*gtkspell\* pygtksourceview\* gnome-doc-utils control-center\* evince\* gnome-user\* gnome-system\* guake gnome-activity-journal gnome-power-manager verbiste-gnome gtg gnome-2048 gnome-klotski gnome-battery-bench gnome-builder gnome-common gnome-dictionary tracker-preferences gedit-plugins gedit-code-assistance gnome-sound-recorder gnome-todo gnome-themes-legacy gnome-shell-extension-auto-move-windows gnome-shell-extension-activities-configurator gnome-shell-extension-freon gnome-shell-theme-* gnome-multi-writer $NO_DEVEL $SKIP_MISSING_PACKAGE

## LibreOffice
dnf -y install libreoffice-calc libreoffice-writer libreoffice-impress libreoffice-math libreoffice-base libreoffice-draw libreoffice-pdfimport $SKIP_MISSING_PACKAGE

## Development
dnf -y groupinstall "Outils de développement et bibliothèques pour C" "Outils système"
dnf -y install gcc gcc-c++ wget nasm subversion git-gui git-tools git-email openssl-devel ncurses-devel ncurses-compat-libs glibc-devel linuxdoc\* kernel-tools kernel-doc kernel-devel kernel-headers python3-sphinx rst2pdf i2c-tools minicom picocom $SKIP_MISSING_PACKAGE

## Buildroot / ptxdist
dnf -y install doxygen vim-common texinfo makeinfo yacc bison flex automake aclocal autoconf glibc.i686 $NO_DEVEL $SKIP_MISSING_PACKAGE

## Yocto
dnf install diffstat chrpath socat SDL-devel xterm docbook-style-dsssl docbook-style-xsl docbook-dtds docbook-utils fop libxslt dblatex xmlto xsltproc autoconf automake libtool glib2-devel libarchive-devel GitPython dosfstools e2fsprogs gawk mtools parted mtd-utils mtd-utils-ubi libusb-devel zlib.i686 lzo.i686 libuuid.i686 libusbx.i686 $SKIP_MISSING_PACKAGE

## Qt development + Sailfish OS
dnf -y install qt5-linguist qt5-designer qt-creator qt5-*-devel VirtualBox $SKIP_MISSING_PACKAGE

## Misc
dnf -y install gajim easytag youtube-dl policycoreutils-gui blivet-gui beignet soundconverter GraphicsMagick theora-tools crack john freetype-freeworld man-pages dejavu\* acpi acpid tor acpitool screen system-config-\* linux_logo fedora-business-cards fedora-easy-karma fedora-packager mediawriter lm_sensors bash-completion bash-doc dvipng libvirt\* qemu\* virt-manager accountsdialog gparted samba-client p7zip\* rpmdevtools nmap wireshark-gtk iperf indent powertop htop iotop bpython python-farsight python-crypto python-virtualenv python-virtualenvwrapper python-pip fedmsg\* gnome-\*fedmsg mediainfo-gui libva-utils vdpauinfo libva-vdpau-driver libva-intel-driver $NO_DEVEL $SKIP_MISSING_PACKAGE

## Games
if [ "$1" != "WORK" ]; then
	dnf -y install springlobby extremetuxracer bzflag clanbomber liquidwar\* fortune-mod frozen-bubble neverball pinball pingus powermanga scorched3d supertux\* torcs\* tuxpuck\* tuxtype2 warmux\* xmoto\* xscorch openalchemist celestia stellarium 0ad teeworlds $NO_DEVEL $SKIP_MISSING_PACKAGE
fi

## Themes
## Alternate wallpapers from Fedora: _\*-backgrounds-extras-gnome gears-backgrounds neon-backgrounds leonidas-backgrounds-landscape_ packages
dnf -y install \*-backgrounds-gnome \*-backgrounds-stripes-gnome desktop-backgrounds-waves solar-backgrounds leonidas-backgrounds leonidas-backgrounds-lion fedorainfinity-backgrounds plymouth-theme-\* bluecurve-gtk-themes bluecurve-cursor-theme bluecurve-icon-theme grub2-starfield-theme grub-customizer $NO_DEVEL $SKIP_MISSING_PACKAGE

## Personal language
dnf install langpacks-en langpacks-$LANGUAGE

# Plymouth theme
## text = colored textual bar
## spinfinity = Fedora and infinite
## fade-in = Fedora with stars
## charge = Fedora charging
## solar = blue solar
## spinner = charging logo like Mac or Firefox style
## hot-dog = Beefy Miracle
## details = text only
plymouth-set-default-theme charge
/usr/libexec/plymouth/plymouth-update-initrd

# Kernel config
echo "kernel.sysrq = 1
vm.swappiness = 15
fs.inotify.max_user_watches = 524288" > /etc/sysctl.conf

# Boot option (add acpi compatibility for backlight) and add theme
sed -i 's/^GRUB_CMDLINE_LINUX="\([^:]*\)"/GRUB_CMDLINE_LINUX="\1 acpi_backlight=vendor"' /etc/default/grub
sed -i 's/GRUB_TERMINAL_OUTPUT="console"/#GRUB_TERMINAL_OUTPUT="console"/' /etc/default/grub
echo "GRUB_THEME=\"/boot/grub2/themes/system/theme.txt\"" >> /etc/default/grub
grub2-mkconfig -o /boot/grub2/grub.cfg

systemctl mask packagekit.service
systemctl disable lvm2-monitor livesys.service

if [ "$1" == "WORK" ]; then
	# Enable NFS 2 and 4.1
	sed -i 's/^RPCNFSDARGS="\([^:]*\)"/RPCNFSDARGS="\1 -V 2 -N 4.1"' /etc/sysconfig/nfs

	# Enable NFS for some development targets
	echo "/home/Renault/NFS *(rw,sync,no_root_squash,no_subtree_check)" > /etc/exports
	exportfs -ar

	firewall-cmd --permanent --zone=FedoraWorkstation --add-port=111/tcp
	firewall-cmd --permanent --zone=FedoraWorkstation --add-port=2049/tcp
	firewall-cmd --permanent --zone=FedoraWorkstation --add-port=20048/tcp
	firewall-cmd --permanent --zone=FedoraWorkstation --add-port=59962/tcp
	firewall-cmd --reload

	echo "[Service]
Environment=\"NSS_HASH_ALG_SUPPORT=+MD5\"
Environment=\"OPENSSL_ENABLE_MD5_VERIFY=1\"" > /etc/systemd/system/NetworkManager.service.d/override.conf

	systemctl daemon-reload
	systemctl restart NetworkManager.service
	systemctl enable fstrim.timer
	systemctl start fstrim.timer
fi
