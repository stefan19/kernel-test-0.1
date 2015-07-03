mkdir /media/floppy1/
mkfs.msdos ../grub/myos.img
mount -o loop ../grub/myos.img /media/floppy1
mkdir /media/floppy1/boot
cp ../grub/stage? /media/floppy1/boot
cp ../grub/menu.cfg /media/floppy1/boot
umount /media/floppy1
rm -r /media/floppy1
