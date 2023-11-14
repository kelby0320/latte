#!/bin/bash

dd if=/dev/zero of=./latte.img bs=1M count=100
echo -en 'n\np\n1\n\n\na\nw\n' | fdisk ./latte.img
sudo losetup /dev/loop0 ./latte.img
sudo losetup /dev/loop1 ./latte.img -o 1048576
sudo mke2fs /dev/loop1
sudo mount /dev/loop1 /mnt/latte
sudo grub-install --root-directory=/mnt/latte --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop0
sudo cp ./boot/grub/grub.cfg /mnt/latte/boot/grub
sudo cp ./build/boot/latte.elf /mnt/latte/boot
sudo umount /mnt/latte
sudo losetup -d /dev/loop0
sudo losetup -d /dev/loop1