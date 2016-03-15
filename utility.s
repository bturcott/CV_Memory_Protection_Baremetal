
.globl __cs3_isr_dabort

.text

__cs3_isr_dabort:
	push {r0,r1,r2}
    ldr r2,=exception_flag
	ldrd r0, [r2]
	adds r0, #1
	adc r1, #0
	strd r0, [r2]
	pop {r0,r1,r2}
	subs pc, lr, #4
