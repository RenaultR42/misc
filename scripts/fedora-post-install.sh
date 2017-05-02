#!/bin/sh

# To customize user profile after that all pice of software are installed
gsettings set org.gnome.software download-updates false
gsettings set org.gnome.software show-upgrade-prerelease true

gsettings set org.gnome.desktop.interface show-battery-percentage true
gsettings set org.gnome.desktop.interface clock-show-date true
gsettings set org.gnome.desktop.interface clock-show-seconds true

gsettings set org.gnome.desktop.sound event-sounds false

gsettings set org.gtk.settings.file-chooser show-hidden true
gsettings set org.gtk.settings.file-chooser sort-directories-first true

gsettings set org.gnome.Music repeat one
gsettings set org.gnome.Totem repeat true

gsettings set org.gnome.builder.code-insight clang-autocompletion true
gsettings set org.gnome.builder.editor draw-spaces ['space', 'tab']
gsettings set org.gnome.builder.editor highlight-current-line true
gsettings set org.gnome.builder.editor highlight-matching-brackets true
gsettings set org.gnome.builder.editor show-map true
gsettings set org.gnome.builder.editor style-scheme-name 'builder-dark'

gsettings set org.gnome.calculator button-mode 'programming'

gsettings set org.gnome.empathy.conversation spell-schecker-enabled true
gsettings set org.gnome.empathy.conversation spell-schecker-languages 'en,fr'
gsettings set org.gnome.empathy.conversation theme 'PlanetGNOME'
gsettings set org.gnome.empathy.sounds sounds-enabled false

gsettings set org.gnome.system.location enabled false
