all : armemu

add.o : add.s
	as -o add.o add.s

sub.o : sub.s
	as -o sub.o sub.s

armemu : armemu.c sub.o
	gcc -o armemu armemu.c sub.o

clean:
	rm -rf armemu sub.o

