.global find_max
.func find_max


find_max:sub sp,sp,#8
	str r4,[sp]

	mov r2,#0
	ldr r3,[r0]
	add r0,r0,#4
	add r2,r2,#1

loop:	cmp r2,r1
	beq done
	ldr r4,[r0]
	cmp r3,r4
	movle r3,r4
	movgt r3,r3
	add r0,r0,#4
	add r2,r2,#1
	b loop

done:	mov r0,r3
	ldr r4,[sp]
	add sp,sp,#8
	bx lr
	
