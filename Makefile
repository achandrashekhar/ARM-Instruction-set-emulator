all : armemu

sum_array.o : sum_array.s
	as -o sum_array.o sum_array.s 

sum_array_real.o : sum_array_real.s
	as -o sum_array_real.o sum_array_real.s

find_max.o : find_max.s
	as -o find_max.o find_max.s

find_str.o : find_str.s
	as -o find_str.o find_str.s

fib_recur.o : fib_recur.s
	as -o fib_recur.o fib_recur.s

fib_iter.o : fib_iter.s
	as -o fib_iter.o fib_iter.s

armemu : armemu.c sum_array_real.o find_max.o fib_recur.o fib_iter.o find_str.o
	gcc -o armemu armemu.c sum_array_real.o find_max.o fib_recur.o fib_iter.o find_str.o

clean:
	rm -rf armemu sum_array_real.o

