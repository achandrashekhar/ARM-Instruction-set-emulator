.global sum_array
.func sum_array

sum_array:
	 mov r2,#0

loop:	cmp r2,r1
	beq done
	add r2,r2,#1
	b loop
	add r2,r2,#1
	add r2,r2,#1
done:   mov r0,r2
	bx lr
	
