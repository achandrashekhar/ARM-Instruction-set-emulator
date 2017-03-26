.global mov
.func mov

mov: 	cmp r0,r1
	movle r0,r1
	movgt r0,r0
	bx lr
