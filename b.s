	.global b
	.func b

b:	
	add r0, r0, r1
	b loop
	add r0,r0,r1
loop:	add r0,r0,r1
	bx lr
