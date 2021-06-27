build: compile
	gcc -o mykernel kernel.o pcb.o ram.o cpu.o shell.o interpreter.o shellmemory.o memorymanager.o DISK_driver.o

compile:
	gcc -c kernel.c pcb.c ram.c cpu.c shell.c interpreter.c shellmemory.c memorymanager.c DISK_driver.c

clean:
	rm -f mysh mykernel *.o
	rm -rf BackingStore
	rm -rf PARTITION