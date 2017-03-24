.global str
.func str

str:	sub sp,sp,#8
	str r1,[sp]
	ldr r0,[sp]
	add sp,sp,#8
	bx lr
