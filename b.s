	.global b
	.func b

b:	
	cmp r0,r1
	beq loop
	add r0,r0,r1
loop:	bx lr