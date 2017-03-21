all : armemu

add.o : add.s
	as -o add.o add.s

sub.o : sub.s
	as -o sub.o sub.s

mov.o :  mov.s
	as -o mov.o mov.s

str.o : str.s
	as -o str.o str.s

armemu : armemu.c str.o
	gcc -o armemu armemu.c str.o

clean:
	rm -rf armemu str.o

