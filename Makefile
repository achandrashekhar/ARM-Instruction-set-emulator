all : armemu

add.o : add.s
	as -o add.o add.s

sub.o : sub.s
	as -o sub.o sub.s

mov.o :  mov.s
	as -o mov.o mov.s

str.o : str.s
	as -o str.o str.s

b.o : b.s
	as -o b.o b.s

sum_array.o : sum_array.s
	as -o sum_array.o sum_array.s 

sum_array_real.o : sum_array_real.s
	as -o sum_array_real.o sum_array_real.s

armemu : armemu.c sum_array_real.o
	gcc -o armemu armemu.c sum_array_real.o

clean:
	rm -rf armemu sum_array_real.o

