#!/usr/bin/env bash
cd ~
git clone git@gitlab.com:alexandreyy/eldshoes.git
wget https://packages.microsoft.com/config/ubuntu/16.04/packages-microsoft-prod.deb 
sudo dpkg -i packages-microsoft-prod.deb
sudo apt-get update
sudo apt-get install blobfuse

sudo mkdir -p /mnt/resource/netshoes
sudo chown grupo2 /mnt/resource/netshoes
mkdir ~/netshoes
blobfuse /home/grupo2/netshoes --tmp-path=/mnt/resource/netshoes  --config-file=/home/grupo2/eldshoes/netshoes.cfg -o attr_timeout=240 -o entry_timeout=240 -o negative_timeout=120

sudo mkdir -p /mnt/resource/grupo2storage2
sudo chown grupo2 /mnt/resource/grupo2storage2
mkdir ~/grupo2storage2
blobfuse /home/grupo2/grupo2storage2 --tmp-path=/mnt/resource/grupo2storage2 --config-file=/home/grupo2/eldshoes/grupo2storage2.cfg -o attr_timeout=240 -o entry_timeout=240 -o negative_timeout=120
