for i in $(find userspace -name "*.c"); do
	gcc -c -ffreestanding -fshort-wchar -mgeneral-regs-only $i -I apps 
done

for i in $(find userspace -name "*.asm"); do
	nasm -felf64 $i 
done


mv userspace/*.o ./
