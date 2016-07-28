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
	dnf config-manager --set-enabled fedora
	dnf config-manager --set-enabled fedora-updates
	dnf config-manager --set-enabled fedora-updates-testing
	dnf config-manager --set-enabled rpmfusion-free
	dnf config-manager --set-enabled rpmfusion-free-updates
	dnf config-manager --set-enabled rpmfusion-free-updates-testing
else
	dnf config-manager --set-enabled fedora-rawhide
	dnf config-manager --set-enabled rpmfusion-rawhide
fi

# Packages
dnf -y upgrade

dnf -y groupinstall "Fedora Workstation"
dnf -y install dnf-plugin-system-upgrade youtube-dl vlc mplayer mplayer-gui mencoder thunderbird\* gstreamer\*-plugins*\ gstreamer1-libav gstreamer1-vaapi ffmpeg gpm xvidcore --exclude=\*docs $NO_DEVEL $SKIP_MISSING_PACKAGE

# To have only real Free Software
#dnf -y remove *firmware* linuxdoc-tools

# GNOME
dnf -y groupinstall "Environnement de bureau standard"
dnf -y install gnome-tweak-tool gnome-weather gitg epiphany swell-foop hitori tali iagno lightsoff quadrapassel four-in-a-row gnome-tetravex gnome-nibbles gnome-taquin gnome-mahjongg gnome-robots gnome-sudoku five-or-more gnome-maps gnome-chess gnome-mines gnome-clocks gnome-boxes polari gnome-music gnome-photos ghex ekiga gnome-tweak-tool gnome-hearts simple-scan gnome-mime-data dconf-editor \*gtkspell\* pygtksourceview\* gnome-doc-utils control-center\* evince\* gnome-user\* gnome-system\* guake gnome-activity-journal gnome-power-manager verbiste-gnome gtg gnome-2048 gnome-klotski gnome-battery-bench gnome-builder gnome-common gnome-dictionary liveusb-creator tracker-preferences gedit-plugins gedit-code-assistance $NO_DEVEL $SKIP_MISSING_PACKAGE

# LibreOffice
dnf -y install libreoffice-calc libreoffice-writer libreoffice-impress libreoffice-math libreoffice-base libreoffice-draw libreoffice-pdfimport $SKIP_MISSING_PACKAGE

# Work
dnf -y groupinstall "Outils de développement et bibliothèques pour C" "Outils système"
dnf -y install gcc gcc-c++ wget nasm git-gui git-tools git-email openssl-devel ncurses-devel ncurses-compat-libs glibc-devel linuxdoc\* kernel-tools kernel-doc kernel-devel kernel-headers python3-sphinx rst2pdf i2c-tools $NO_DEVEL $SKIP_MISSING_PACKAGE
dnf -y install fusioninventory-agent minicom doxygen vim-common texinfo makeinfo yacc bison flex automake aclocal autoconf glibc.i686 $NO_DEVEL $SKIP_MISSING_PACKAGE

# Yocto
dnf install diffstat chrpath socat SDL-devel xterm docbook-style-dsssl docbook-style-xsl docbook-dtds docbook-utils fop libxslt dblatex xmlto xsltproc autoconf automake libtool glib2-devel libarchive-devel GitPython

dnf -y install gajim easytag policycoreutils-gui blivet-gui beignet soundconverter GraphicsMagick theora-tools crack john freetype-freeworld man-pages dejavu\* acpi tor acpitool screen stardict-dic-en system-config-\* linux_logo fedora-business-cards fedora-easy-karma fedora-packager livecd-tools lm_sensors bash-completion bash-doc dvipng libvirt\* qemu\* virt-manager accountsdialog gparted samba-client p7zip\* rpmdevtools nmap wireshark iperf indent powertop htop iotop bpython python-farsight python-crypto python-virtualenv python-virtualenvwrapper python-pip fedmsg\* gnome-\*fedmsg mediainfo-gui libva-utils vdpauinfo libva-vdpau-driver libva-intel-driver $NO_DEVEL $SKIP_MISSING_PACKAGE

# Games
dnf -y install springlobby extremetuxracer bzflag clanbomber liquidwar\* fortune-mod frozen-bubble neverball pinball pingus powermanga scorched3d supertux\* torcs\* tuxpuck\* tuxtype2 warmux\* xmoto\* xscorch openalchemist celestia stellarium 0ad teeworlds $NO_DEVEL $SKIP_MISSING_PACKAGE

# Themes
dnf -y install \*-backgrounds-gnome solar-backgrounds astronomy-backgrounds gears-backgrounds constantine-backgrounds fedorainfinity-backgrounds desktop-backgrounds-waves neon-backgrounds leonidas\* plymouth-theme-\* bluecurve-gtk-themes bluecurve-cursor-theme bluecurve-icon-theme grub2-starfield-theme grub-customizer $NO_DEVEL $SKIP_MISSING_PACKAGE

# personnal language
dnf langinstall $LANGUAGE
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
vm.swappiness=0
vm.laptop_mode = 5
vm.dirty_writeback_centisecs = 3000
vm.dirty_expire_centisecs = 3000" > /etc/sysctl.conf

# Boot option (add acpi compatibility for backlight) and add theme
sed -i 's/^GRUB_CMDLINE_LINUX="\([^:]*\)"/GRUB_CMDLINE_LINUX="\1 acpi_backlight=vendor"' /etc/default/grub
sed -i 's/GRUB_TERMINAL_OUTPUT="console"/#GRUB_TERMINAL_OUTPUT="console"/' /etc/default/grub
echo "GRUB_THEME=\"/boot/grub2/themes/system/theme.txt\"" >> /etc/default/grub
grub2-mkconfig -o /boot/grub2/grub.cfg

if [ "$1" == "WORK" ]; then
	# Enable NFS 2 and 4.1
	sed -i 's/^RPCNFSDARGS="\([^:]*\)"/RPCNFSDARGS="\1 -V 2 -N 4.1"' /etc/sysconfig/nfs

	# Enable NFS for some development targets
	echo "/home/Renault/Documents/Antares/code/sw/buildroot/output/images/ 192.168.11.11(rw,sync,no_root_squash,no_subtree_check)
/home/Renault/Documents/Antares/code/sw/buildroot/output/nfs 192.168.11.11(rw,sync,no_root_squash,no_subtree_check)" > /etc/exports
	exportfs -ar

	firewall-cmd --permanent --zone=FedoraWorkstation --add-port=111/tcp
	firewall-cmd --permanent --zone=FedoraWorkstation --add-port=2049/tcp
	firewall-cmd --permanent --zone=FedoraWorkstation --add-port=20048/tcp
	firewall-cmd --permanent --zone=FedoraWorkstation --add-port=59962/tcp
	firewall-cmd --reload
fi
