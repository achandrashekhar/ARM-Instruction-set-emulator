all : armemu

add.o : add.s
	as -o add.o add.s

sub.o : sub.s
	as -o sub.o sub.s

mov.o :  mov.s
	as -o mov.o mov.s


armemu : armemu.c mov.o
	gcc -o armemu armemu.c mov.o

clean:
	rm -rf armemu mov.o

