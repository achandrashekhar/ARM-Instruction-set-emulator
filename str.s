.global str
.func str

str:
	str r1, [r4]
	ldr r0, [r4]
	bx lr
