	.globl fib_iter
	.func fib_iter

fib_iter:
	mov r4,#2
	mov r1,#0
	mov r2,#1
	cmp r0,#0
	beq return_a0

loop:	cmp r4,r0
	beq done2
	add r3,r1,r2
	mov r1,r2
	mov r2,r3
	add r4,r4,#1
	b loop
	
return_a0:
	mov r0,#0
	bx lr	
done2:	mov r0,r2
	bx lr
