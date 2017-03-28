.globl fib_recur
	.func fib_recur

fib_recur:
	sub sp,sp,#16
	str lr,[sp]
	cmp r0,#2
	blt done
	add sp,sp,#4
	str r0,[sp]
	sub sp,sp,#4
	sub r0,r0,#1
	bl fib_recur
	add sp,sp,#8
	str r0,[sp]
	sub sp,sp,#8
	add sp,sp,#4
	ldr r0,[sp]
	sub sp,sp,#4
	sub r0,r0,#2
	bl fib_recur
	add sp,sp,#8
	ldr r1,[sp]
	sub sp,sp,#8
	add r0,r0,r1

done:
	ldr lr,[sp]
	add sp,sp,#16
	bx lr
