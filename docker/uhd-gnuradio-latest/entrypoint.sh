#!/bin/bash

USER_ID=${LOCAL_UID:-9001}
GROUP_ID=${LOCAL_GID:-9001}

echo "Starting with UID: $USER_ID, GID: $GROUP_ID"
useradd -u $USER_ID --create-home --shell /bin/bash -o titan -G sudo
groupmod -g $GROUP_ID titan
echo 'titan:titan' | chpasswd
export HOME=/home/titan

# copy Xauthority so that X11 works over SSH
cp /root/.Xauthority $HOME/
chown titan:titan $HOME/.Xauthority

# Change to home and modify PS1 to show RED hostname (a hint you are in the docker image) and git branch
cd $HOME
cat  << 'EOF' >> .bashrc
parse_git_branch() {
    git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'
}
export PS1='${debian_chroot:+($debian_chroot)}\[\033[01;31m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$$(parse_git_branch) '
EOF

# Got to the source directory automatically
cd /ihd

# Be titan now
exec /usr/sbin/gosu titan "$@"
