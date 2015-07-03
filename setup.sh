mkdir /media/floppy1
mount -o loop ../grub/myos.img /media/floppy1
cp kernel /media/floppy1/kernel
umount /media/floppy1
rm -r /media/floppy1
