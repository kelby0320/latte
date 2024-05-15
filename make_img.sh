#!/bin/bash

GRUB_LOOP=/dev/loop0
EXT2_LOOP=/dev/loop1

dd if=/dev/zero of=./latte.img bs=1M count=100
echo -en 'n\np\n1\n\n\na\nw\n' | fdisk ./latte.img
sudo losetup $GRUB_LOOP ./latte.img
sudo losetup $EXT2_LOOP ./latte.img -o 1048576
sudo mke2fs $EXT2_LOOP
sudo mount $EXT2_LOOP /mnt/latte
sudo grub-install --root-directory=/mnt/latte --target=i386-pc --no-floppy --modules="normal part_msdos ext2 multiboot" $GRUB_LOOP
sudo cp ./boot/grub/grub.cfg /mnt/latte/boot/grub
sudo cp ./boot/latte-0.1 /mnt/latte/boot
sudo mkdir /mnt/latte/bin
sudo cp ./bin/hello/hello /mnt/latte/bin
sudo cp ./bin/init/init /mnt/latte/bin
sudo mkdir /mnt/latte/lib
sudo cp ./lib/libc/libc.a /mnt/latte/lib
sudo cp ./latte.txt /mnt/latte
sudo umount /mnt/latte
sudo losetup -d $GRUB_LOOP
sudo losetup -d $EXT2_LOOP