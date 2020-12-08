/*
 * asm.s
 *
 * author: Furkan Cayci 
 *
 * description: Added the necessary stuff for turning on the green LED on the 
 *   G031K8 Nucleo board. Mostly for teaching.
 */


.syntax unified
.cpu cortex-m0plus
.fpu softvfp
.thumb


/* make linker see this */
.global Reset_Handler

/* get these from linker script */
.word _sdata
.word _edata
.word _sbss
.word _ebss


/* define peripheral addresses from RM0444 page 57, Tables 3-4 */
.equ RCC_BASE,         (0x40021000)          // RCC base address
.equ RCC_IOPENR,       (RCC_BASE   + (0x34)) // RCC IOPENR register offset

.equ GPIOA_BASE,       (0x50000000)          // GPIOA base address
.equ GPIOA_MODER,      (GPIOA_BASE + (0x00)) // GPIOA MODER register offset
.equ GPIOA_IDR,        (GPIOA_BASE + (0x10)) // GPIOA IDR register offset
.equ GPIOA_ODR,        (GPIOA_BASE + (0x14)) // GPIOA ODR register offset

.equ GPIOB_BASE,       (0x50000400)          // GPIOB base address
.equ GPIOB_MODER,      (GPIOB_BASE + (0x00)) // GPIOB MODER register offset
.equ GPIOB_ODR,        (GPIOB_BASE + (0x14)) // GPIOB ODR register offset

.equ GPIOC_BASE,       (0x50000800)          // GPIOC base address
.equ GPIOC_MODER,      (GPIOC_BASE + (0x00)) // GPIOC MODER register offset
.equ GPIOC_ODR,        (GPIOC_BASE + (0x14)) // GPIOC ODR register offset

.equ SEGMENT_VALUE_RAM_ADD,		(_estack + (0x00))    // Segment_Values_Global_Ram_Adress
.equ LAST_ACTIVE_SEGMENT_ADD,	(_estack + (0x04))    // Last_Active_Segment_Global_Ram_Adress
.equ FLAG_DECRMT_CON_ADD,		(_estack + (0x08))    // Flag_Decrement_Condition_Global_Ram_Adress

.equ IDLE_STATE,		0
.equ COUNTDOWN_STATE,	1

SevenSegmentLookUpTable: .byte 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F
.text

/* vector table, +1 thumb mode */
.section .vectors
vector_table:
	.word _estack             /*     Stack pointer */
	.word Reset_Handler +1    /*     Reset handler */
	.word Default_Handler +1  /*       NMI handler */
	.word Default_Handler +1  /* HardFault handler */
	/* add rest of them here if needed */


/* reset handler */
.section .text
Reset_Handler:
	/* set stack pointer */
	ldr r0, =_estack
	mov sp, r0

	/* initialize data and bss 
	 * not necessary for rom only code 
	 * */
	bl init_data
	/* call main */
	bl main
	/* trap if returned */
	b .


/* initialize data and bss sections */
.section .text
init_data:

	/* copy rom to ram */
	ldr r0, =_sdata
	ldr r1, =_edata
	ldr r2, =_sidata
	movs r3, #0
	b LoopCopyDataInit

	CopyDataInit:
		ldr r4, [r2, r3]
		str r4, [r0, r3]
		adds r3, r3, #4

	LoopCopyDataInit:
		adds r4, r0, r3
		cmp r4, r1
		bcc CopyDataInit

	/* zero bss */
	ldr r2, =_sbss
	ldr r4, =_ebss
	movs r3, #0
	b LoopFillZerobss

	FillZerobss:
		str  r3, [r2]
		adds r2, r2, #4

	LoopFillZerobss:
		cmp r2, r4
		bcc FillZerobss

	bx lr


/* default handler */
.section .text
Default_Handler:
	b Default_Handler


/* main function */
.section .text
main:

	/* enable GPIOA - GPIOB clock, bit0 and bit1 on IOPENR */
	ldr r6, =RCC_IOPENR
	ldr r5, [r6]
	/* movs expects imm8, so this should be fine */
	movs r4, 0x07
	orrs r5, r5, r4
	str r5, [r6]

	/* setup AllPins output for portB pins we clear all bits in MODER */
	ldr r6, =GPIOB_MODER
	ldr r5, [r6]
	/* cannot do with movs, so use pc relative */
	ldr r4, =0x55555555
	str r4, [r6]

	/* setup A0 for external button, other pins are output. */
	ldr r6, =GPIOA_MODER
	ldr r5, [r6]
	ldr r4, =0x7FD5FFFC
	ands r4, r5
	str r4, [r6]

	/* setup AllPins output for portB pins we clear all bits in MODER */
	ldr r6, =GPIOC_MODER
	ldr r5, [r6]
	/* cannot do with movs, so use pc relative */
	ldr r4, =0x55555555
	str r4, [r6]

	/* clear all outputs before start */
	ldr r6, =GPIOA_ODR
	ldr r4, =0
	str r4, [r6]
	ldr r6, =GPIOB_ODR
	ldr r4, =0
	str r4, [r6]
	ldr r6, =GPIOC_ODR
	ldr r4, =0
	str r4, [r6]

	movs r0,IDLE_STATE
	movs r7, #0

	ldr r6, =LAST_ACTIVE_SEGMENT_ADD
	movs r4, #0
	str r4, [r6]

	ldr r6, =FLAG_DECRMT_CON_ADD
	ldr r4, =0x01
	str r4, [r6]

	movs r4, #0
	mov r8,r4

loop:
	/* Check Actual State*/
	cmp r0,IDLE_STATE
	beq ChangeStateIdle
	bl CounterDownStateTask

	b loop

ChangeStateIdle:
	bl IdleStateTask
	b loop

IncreaseIterationNumber:
	push { LR }
	movs r7, #1
	add r8, r7
	ldr r7, =1000
	cmp r8, r7
	bls ExitIteration
	movs r4, #0
	mov r8, r4
ExitIteration:
	pop { PC }

IdleStateTask:
	push { LR }
	ldr r6, =SEGMENT_VALUE_RAM_ADD
	ldr r4, =0x02020906
	str r4, [r6]

	bl IncreaseIterationNumber
	ldr r6, =GPIOA_IDR
	ldr r5, [r6]
	movs r4, #0x01
	/* If we press button RA0 is clear */
	ands r5, r4
	beq IdleButtonPressed
	bl Update_Segment
	bl Delay1Ms
	pop { PC }

IdleButtonPressed:
	push { LR }
	/* 50 ms wait and check again*/
	ldr r2, =50
WaitButton:
	bl Update_Segment
	bl Delay1Ms
	subs r2, r2, #1
	bne WaitButton
	/*Check again */
	ldr r6, =GPIOA_IDR
	ldr r5, [r6]
	movs r4, #0x01
	/* If we press button RA0 is clear */
	ands r5, r4
	bne ExitIdleButton
WaitButton2:
	bl Update_Segment
	bl Delay1Ms
	/* Wait Button Release */
	ldr r6, =GPIOA_IDR
	ldr r5, [r6]
	movs r4, #0x01
	/* If we press button RA0 is clear */
	ands r5, r4
	beq WaitButton2

	bl Generate_Random
	#if 0
	/*Update Segment Value on RAM*/
	ldr r6, =SEGMENT_VALUE_RAM_ADD
	ldr r4, =0x09000000
	str r4, [r6]
	#endif
	ldr r6, =FLAG_DECRMT_CON_ADD
	ldr r4, =0x01
	str r4, [r6]
	/* Change State To Countdown */
	movs r0, COUNTDOWN_STATE
	/* Activate External Led */
	ldr r6, =GPIOC_ODR
	movs r4, #0x40
	str r4, [ r6 ]
	bl ExitIdleButton
ExitIdleButton:
	pop { PC }

Generate_Random:
	push { LR }
	movs r2, #1
	movs r3, #2	// A value
	movs r4, #3	// C value
	ldr r5, =7999 // M value
RepeatIteration:
	muls r2, r2, r3
	adds r2, r4
TakeMod:
	cmp r2, r5
	bls NextIteration
	subs r2, r2, r5
	b TakeMod
NextIteration:
	mov r6, r8
	movs r7, #1
	subs r6, r6, r7
	mov r8, r6
	movs r6, #0
	cmp r8, r6
	bne	RepeatIteration
ResultRandom:
	ldr r4, =1000
	adds r2, r2, r4

	/* Decimal to BCD for update digits */
	movs r4, #0
	movs r5, #0
FirstDigit:
	ldr r3, =1000
	cmp r2, r3
	blt RecordFirst
	subs r2, r2, r3
	adds r4, #1
	b FirstDigit
RecordFirst:
	/*Update Segment Value on RAM*/
	ldr r6, =SEGMENT_VALUE_RAM_ADD
	movs r5, r4
	strb r5, [r6, 3]
	movs r4, #0
	movs r5, #0
SecondDigit:
	movs r3, #100
	cmp r2, r3
	blt RecordSecond
	subs r2, r2, r3
	adds r4, #1
	b SecondDigit
RecordSecond:
	movs r5, r4
	strb r5, [r6, 2]
	movs r4, #0
	movs r5, #0
ThirdDigit:
	movs r3, #10
	cmp r2, r3
	blt RecordThird
	subs r2, r2, r3
	adds r4, #1
	b ThirdDigit
RecordThird:
	movs r5, r4
	strb r5, [r6, 1]
	movs r4, #0
	movs r5, #0
ForthDigit:
	strb r2, [r6, 0]

	pop { PC }

CounterDownStateTask:
	push { LR }

	bl IncreaseIterationNumber
	/* Check Button Pressed */
	ldr r6, =GPIOA_IDR
	ldr r5, [r6]
	movs r4, #0x01
	/* If we press button RA0 is clear */
	ands r5, r4
	beq CounterButtonPressed
CounterPressedFinished:
	bl Update_Segment
	ldr r6, =FLAG_DECRMT_CON_ADD
	ldr r5, [r6]
	cmp r5, 0x00
	bne DecrementCounter
	b JumpToCounterWait
DecrementCounter:
	/* Check Counter is zero */
	ldr r6, =SEGMENT_VALUE_RAM_ADD
	ldr r7, [r6]
	cmp r7, #0
	beq Wait1Second
	/* Decrement Bytes */
	ldr r6, =SEGMENT_VALUE_RAM_ADD
	ldrb r7, [r6, 0]
	cmp r7, #0
	beq Check_other_1
	subs r7, #1
	strb r7, [r6, 0]
	b JumpToCounterWait
Check_other_1:
	ldrb r7, [r6, 1]
	cmp r7, #0
	beq Check_other_2
	subs r7, #1
	strb r7, [r6, 1]
	cmp r7, #0
	beq JumpToCounterWait
	movs r7, #9
	strb r7, [r6, 0]
	b JumpToCounterWait
Check_other_2:
	ldrb r7, [r6, 2]
	cmp r7, #0
	beq Check_other_3
	subs r7, #1
	strb r7, [r6, 2]
	cmp r7, #0
	beq JumpToCounterWait
	movs r7, #9
	strb r7, [r6, 1]
	strb r7, [r6, 0]
	b JumpToCounterWait
Check_other_3:
	ldrb r7, [r6, 3]
	subs r7, #1
	strb r7, [r6, 3]
	movs r7, #9
	strb r7, [r6, 2]
	strb r7, [r6, 1]
	strb r7, [r6, 0]
	b JumpToCounterWait
JumpToCounterWait:
	bl Delay1Ms
	pop { PC }

CounterButtonPressed:
	/* 50 ms wait and check again*/
	ldr r2, =50
WaitCounterButton:
	bl Update_Segment
	bl Delay1Ms
	subs r2, r2, #1
	bne WaitCounterButton
	/* Check again */
	ldr r6, =GPIOA_IDR
	ldr r5, [r6]
	movs r4, #0x01
	/* If we press button RA0 is clear */
	ands r5, r4
	bne CounterPressedFinished
WaitCounterButton2:
	bl Update_Segment
	bl Delay1Ms
	/* Wait Button Release */
	ldr r6, =GPIOA_IDR
	ldr r5, [r6]
	movs r4, #0x01
	/* If we press button RA0 is clear */
	ands r5, r4
	beq WaitCounterButton2

	/* Invert Decrement Condition Flag */
	ldr r6, =FLAG_DECRMT_CON_ADD
	ldr r5, [r6]
	movs r4, #1
	eors r5, r5, r4
	str r5, [r6]
	b CounterPressedFinished

Wait1Second:
	ldr r2, =1000
Wait1SecondLoop:
	bl Update_Segment
	bl Delay1Ms
	subs r2, r2, #1
	bne Wait1SecondLoop
	b ExitCountDownState
ExitCountDownState:
	/* Change State To Idle */
	movs r0, IDLE_STATE
	/* Passive External Led */
	ldr r6, =GPIOC_ODR
	movs r4, #0
	str r4, [ r6 ]

	pop { PC }

ClearCounter:
	movs r7, 0
	bx lr

Update_Segment:
	push { LR }
	/*Active Segment Reset*/
	ldr r6, =LAST_ACTIVE_SEGMENT_ADD
	ldr r5, [r6]
	cmp r5, 0x00
	bne next_1
	/* Clear Segment 1*/
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	ldr r4, =0xFFFFFEFF
	ands r4, r5
	str r4, [r6]
	movs r5, 1
	bl next_4	/* We found active segment */
next_1:
	cmp r5, 0x01
	bne next_2
	/* Clear Segment 2*/
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	ldr r4, =0xFFFFFDFF
	ands r4, r5
	str r4, [r6]
	movs r5, 2
	bl next_4	/* We found active segment */
next_2:
	cmp r5, 0x02
	bne next_3
	/* Clear Segment 3*/
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	ldr r4, =0xFFFFFBFF
	ands r4, r5
	str r4, [r6]
	movs r5, 3
	bl next_4	/* We found active segment */
next_3:
	cmp r5, 0x03
	bne next_4
	/* Clear Segment 4*/
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	ldr r4, =0xFFFF7FFF
	ands r4, r5
	str r4, [r6]
	movs r5, 0
	bl next_4	/* We found active segment */
next_4:
	/* Update Segment Value */
	ldr r6, =SEGMENT_VALUE_RAM_ADD
	ldrb r7, [r6, r5]
	ldr r3, = SevenSegmentLookUpTable
	ldrb r4, [r3, r7]
	ldr r6, =GPIOB_ODR
	strb r4, [r6, 0]

	/*Activate Segment Digit X*/
	ldr r6, =GPIOA_ODR
	cmp r5, 0x00
	bne Other_Segment_1
	ldr r4, = 0x8000
	str r4, [r6]
	bl Exit_Segment
Other_Segment_1:
	cmp r5, 0x01
	bne Other_Segment_2
	ldr r4, =0x400
	str r4, [r6]
	bl Exit_Segment
Other_Segment_2:
	cmp r5, 0x02
	bne Other_Segment_3
	ldr r4, =0x200
	str r4, [r6]
	bl Exit_Segment
Other_Segment_3:
	ldr r4, =0x100
	str r4, [r6]
	bl Exit_Segment

Exit_Segment:
	/* Update Next Segment*/
	ldr r6, =LAST_ACTIVE_SEGMENT_ADD
	str r5, [r6]
	POP { PC }

Delay1Ms:
	push { LR }
	ldr r1, =0x1F40	/* For 1Ms = 24M/3cycle / 8000*/
delayloop:
	subs r1, r1, #1
	bne delayloop
	pop { PC }

	/* for(;;); */
	b .

	/* this should never get executed */
	nop

