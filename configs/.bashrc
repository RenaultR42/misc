# .bashrc

HISTCONTROL=ignorespace

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# Display the hours in start prompt line
PS1="[\t] \[\e[01;32m\]\u@\h\[\e[00m\]:\[\e[01;34m\]\w\[\e[00m\]\$ "

export PATH=$PATH:/home/Renault/Logiciels/kernel-tests/:/home/Renault/bin/
export HISTCONTROL=ignoredups
export EDITOR=gedit

# Supporte resizing terminals  (xterm and screen -r)
shopt -s checkwinsize

alias ll='ls -lah'
alias cp='cp -p'
