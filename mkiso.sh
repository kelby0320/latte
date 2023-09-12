#!/bin/bash

mkdir -p ./iso/boot/grub
cp ./build/boot/latte.bin ./iso/boot
cp ./boot/grub/grub.cfg ./iso/boot/grub
grub-mkrescue /usr/lib/grub/i386-pc -o latte.iso ./iso
