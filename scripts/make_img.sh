#!/bin/bash

BUILD_DIR="./build"
GRUB_LOOP=/dev/loop0
EXT2_LOOP=/dev/loop1

GRUB_CFG=./boot/grub/grub.cfg
KERNEL=$BUILD_DIR/sys/latte-0.1
BIN_MANIFEST=$(find $BUILD_DIR/bin -type f -regex ".*/[a-zA-Z]+")
LIB_MANIFEST=$(find $BUILD_DIR/lib -type f -regex ".*/[a-zA-Z]+\.a")

# Create and format image file
dd if=/dev/zero of=./latte.img bs=1M count=100
echo -en 'n\np\n1\n\n\na\nw\n' | fdisk ./latte.img
sudo losetup $GRUB_LOOP ./latte.img
sudo losetup $EXT2_LOOP ./latte.img -o 1048576
sudo mke2fs $EXT2_LOOP
sudo mount $EXT2_LOOP /mnt/latte
sudo grub-install --root-directory=/mnt/latte --target=i386-pc --no-floppy --modules="normal part_msdos ext2 multiboot" $GRUB_LOOP

# Copy Grub and the kernel
echo "Copying Grub config and kernel"
sudo cp $GRUB_CFG /mnt/latte/boot/grub
sudo cp $KERNEL /mnt/latte/boot

# Copy programs
echo "Copying bin"
sudo mkdir /mnt/latte/bin
for file in $BIN_MANIFEST
do
    sudo cp $file /mnt/latte/bin
done
    
# Copy libraries
echo "Copying lib"
sudo mkdir /mnt/latte/lib
for file in $LIB_MANIFEST
do
    sudo cp $file /mnt/latte/lib
done

# Copy etc
echo "Copying etc"
sudo mkdir /mnt/latte/etc
sudo cp -r ./etc/* /mnt/latte/etc

sudo umount /mnt/latte
sudo losetup -d $GRUB_LOOP
sudo losetup -d $EXT2_LOOP

echo "Image creation successful"
