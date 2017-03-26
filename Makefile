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

find_max.o : find_max.s
	as -o find_max.o find_max.s

find_str.o : find_str.s
	as -o find_str.o find_str.s

armemu : armemu.c find_str.o
	gcc -o armemu armemu.c find_str.o

clean:
	rm -rf armemu sum_array_real.o

