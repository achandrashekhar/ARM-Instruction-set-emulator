	.global find_str
	.func find_str

find_str:		mov r9,#0
	mov r8,#0

loop:		ldrb r3,[r0,r9]
	cmp r3,#0
	beq loop2
	add r9,r9,#1
	b loop

loop2:		ldrb r4,[r1,r8]
	cmp r4,#0
	beq done_length2
	add r8,r8,#1
	b loop2

done_length2:		sub r8,r8,#1

	mov r2,#0
	mov r3,#0
	mov r4,#-1

loop3:		cmp r2,r9
	beq done
	ldrb r5,[r0,r2]
	ldrb r6,[r1,r3]
	cmp r5,r6
	beq inner_if
	mov r3,#0
	b inc
inner_if:		cmp r3,r8
	beq sub_i_j
	add r3,r3,#1
inc:		add r2,r2,#1
	b loop3

sub_i_j:	sub r4, r2,r3
done:		mov r0,r4
	bx lr
