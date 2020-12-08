/*
 * asm.s
 *
 * author: Furkan Cayci
 * editor: M.Safa BULAT
 * number: 141024051
 *
 * description: I edited this template for my "Project1, 4xSSD" work from Furkan Cayci.
 *
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
.equ GPIOB_BASE,       (0x50000400)          // GPIOB base address
.equ GPIOC_BASE,       (0x50000800)          // GPIOC base address
.equ GPIOD_BASE,       (0x50000C00)          // GPIOD base address


.equ GPIOA_MODER,      (GPIOA_BASE + (0x00)) // GPIOA MODER register offset
.equ GPIOA_ODR,        (GPIOA_BASE + (0x14)) // GPIOA ODR register offset
.equ GPIOA_IDR,        (GPIOA_BASE + (0x10)) // GPIOA IDR register offset

.equ GPIOB_MODER,      (GPIOB_BASE + (0x00)) // GPIOB MODER register offset
.equ GPIOB_ODR,        (GPIOB_BASE + (0x14)) // GPIOB ODR register offset

.equ GPIOC_MODER,      (GPIOC_BASE + (0x00)) // GPIOC MODER register offset
.equ GPIOC_ODR,        (GPIOC_BASE + (0x14)) // GPIOC ODR register offset

.equ PORT_PINS,				(0xFFFF) //That allows to use pins from 0 to 7
.equ CONFasOUTPUT,			(0x5555) //That configures 0-7 pins as output
.equ PIN_7,					(0xC000)  //PIN7 For button

.equ SEG1,					(0x1)	//1.SSD of 4xSSD
.equ SEG2,					(0x2)	//2.SSD of 4xSSD
.equ SEG3,					(0x10)	//3.SSD of 4xSSD
.equ SEG4,					(0x20)	//4.SSD of 4xSSD
.equ LED,					(0x40)	//Green LED

.equ	dig0,				(0x3F)	//Predefine '0' digit
.equ	dig1,				(0x6)	//Predefine '1' digit
.equ	dig2,				(0x5B)	//Predefine '2' digit
.equ	dig3,				(0x4F)	//Predefine '3' digit
.equ	dig4,				(0x66)	//Predefine '4' digit
.equ	dig5,				(0x6D)	//Predefine '5' digit
.equ	dig6,				(0x7D)	//Predefine '6' digit
.equ	dig7,				(0x7)	//Predefine '7' digit
.equ	dig8,				(0x7F)	//Predefine '8' digit
.equ	dig9,				(0x67)	//Predefine '9' digit
.equ	letS,				(0x6D)	//Predefine 'S' letter
.equ	letA,				(0x77)	//Predefine 'A' letter
.equ	letF,				(0x71)	//Predefine 'F' letter

//Array for parser and counter:
arr: .4byte   dig0, dig1, dig2, dig3, dig4, dig5, dig6, dig7, dig8, dig9

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
	/* enable GPIOC clock, bit2 on IOPENR */
	ldr r6, =RCC_IOPENR	//RCC
	ldr r5, [r6]
	movs r4, 0x3		//Activate clock for port A and port B
	orrs r5, r5, r4
	str r5, [r6]

	/* setup PINs for SSD in MODER */
	ldr r6, =GPIOA_MODER						//GPIOA pins
	ldr r5, [r6]
	ldr r4, =PORT_PINS							//Activate 0-8 PINS
	mvns r4, r4
	ands r5, r5, r4
	ldr r4, =CONFasOUTPUT						//Set activated pins as OUTPUT
	orrs r5, r5, r4
	str r5, [r6]								//S1S2S3S4 leds are ready for SSD

	ldr r6, =GPIOB_MODER						//GPIOB pins
	ldr r5, [r6]
	ldr r4, =PORT_PINS							//Activate 0-8 PINS
	mvns r4, r4
	ands r5, r5, r4
	ldr r4, =CONFasOUTPUT						//Set activated pins as OUTPUT
	orrs r5, r5, r4
	str r5, [r6]								//ABCDEFG leds are ready for SSD


	/* PIN configuration for SSD ends  */

	/* setup PA7 for button 00 for bits 14-15 in MODER*/
	ldr r6, = GPIOA_MODER
	ldr r5, [r6]

	ldr r4, = PIN_7	//Set pin to PA7
	mvns r4, r4
	ands r5,r5,r4
	str r5,[r6]
	/* BUTTON configuration ends */


button_ctrl: /* Button statements */

	ldr r6, =GPIOA_IDR //configure as input
	ldr r5, [r6]
	lsrs r5,r5, #7		//pin7
	movs r4, 0x1		//set
	ands r5,r5,r4

	cmp r5, 0x0
	beq counter	//If button==0 go to Statement1 (counter phase)
	bne standby //If button==1 go to Statement2 (standby phase)

counter: //Statement1's function

	/***************************************TURN_ON: Green LED****************************/
	bl greenled_on //Call green led ON function
	/***************************************TURN_ON: Green LED 'ENDS**********************/
	///

	///
	/**********************************************SSD SEG1********************************/
	bl digit1	//call predefined digit 1 function for SEG1 as 'S'
	/**********************************************SSD SEG1 'ENDS**************************/
	///
	bl delay_1s	//ms Delay
	bl seg1_off //Call seg1off function to close that segment
	bl clear	//clear the cells
	///
	/**********************************************SSD SEG2********************************/
	bl digit2	//call predefined digit 2 function for SEG2 as 'A'
	/**********************************************SSD SEG2 'ENDS**************************/
	///
	bl delay_1s
	bl seg2_off
	bl clear
	///
	/**********************************************SSD SEG3********************************/
	bl digit3	//call predefined digit 3 function for SEG3 as 'F'
	/**********************************************SSD SEG3 'ENDS**************************/
	///
	bl delay_1s
	bl seg3_off
	bl clear
	///
	/**********************************************SSD SEG4********************************/
	bl digit4	//call predefined digit 4 function for SEG4 as 'A'
	/**********************************************SSD SEG4 'ENDS**************************/
	///
	bl delay_1s
	bl seg4_off
	bl clear
	///
	/****************************************************BUTTON***************************/
	movs r5, #0			//Clear the value in button

	ldr r6, =GPIOA_IDR	//Call input
	ldr r5, [r6]
	lsrs r5,r5, #7		//From pin 7
	movs r4, 0x1
	ands r5,r5,r4

	cmp r5, 0x0			//Compare input val with '0'
	beq standby	//If button==1 ; go to standby phase
	bne counter //If button==0 ; loop
	/**********************************************BUTTON 'ENDS***************************/
	///

	///

standby: //Statement2's function

	/***************************************TURN_OFF: Green LED****************************/
	bl greenled_off
	/***************************************TURN_OFF: Green LED 'ENDS**********************/
	///
	bl clear
	///
	/**********************************************SSD SEG1********************************/
	ldr r6, =GPIOA_ODR		//Output from port A
	ldr r5, [r6]
	movs r4, SEG1 			//choose SEG1
	orrs r5, r5, r4			//turn-on SEG1
	str r5, [r6]

	ldr r6, =GPIOB_ODR		//Output from port B
	ldr r5, [r6]
	movs r4, dig1			//choose cells from predefined values
	mvns r4, r4
	ands r5, r5, r4			//send logic leves
	str r5, [r6]
	/**********************************************SSD SEG1 'ENDS**************************/
	///
	bl delay_1s
	bl seg1_off
	bl clear
	///
	/**********************************************SSD SEG2********************************/
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG2 //light up
	orrs r5, r5, r4
	str r5, [r6]

	ldr r6, =GPIOB_ODR
	ldr r5, [r6]
	movs r4, dig4
	mvns r4, r4    //if not pressed
	ands r5, r5, r4
	str r5, [r6]
	/**********************************************SSD SEG2 'ENDS**************************/
	///
	bl delay_1s
	bl seg2_off
	bl clear
	///
	/**********************************************SSD SEG3********************************/
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG3 //light up
	orrs r5, r5, r4
	str r5, [r6]

	ldr r6, =GPIOB_ODR
	ldr r5, [r6]
	movs r4, dig5
	mvns r4, r4    //if not pressed
	ands r5, r5, r4
	str r5, [r6]
	/**********************************************SSD SEG3 'ENDS**************************/
	///
	bl delay_1s
	bl seg3_off
	bl clear
	///
	/**********************************************SSD SEG4********************************/
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG4 //light up
	orrs r5, r5, r4
	str r5, [r6]

	ldr r6, =GPIOB_ODR
	ldr r5, [r6]
	movs r4, dig1
	mvns r4, r4    //if not pressed
	ands r5, r5, r4
	str r5, [r6]
	/**********************************************SSD SEG4 'ENDS**************************/
	///
	bl delay_1s
	bl seg4_off
	bl clear
	///
	/****************************************************BUTTON***************************/
	movs r5, #0

	ldr r6, =GPIOA_IDR
	ldr r5, [r6]
	lsrs r5,r5, #7
	movs r4, 0x1
	ands r5,r5,r4

	cmp r5, 0x1
	beq standby	//Statement1
	bne counter //Statement2

	/**********************************************BUTTON 'ENDS***************************/
	///

	///

	/*********************************************END of MAIN*****************************/

	/*********************************************Functions*******************************/

delay_1s://****************************Delay Function****************//
	movs r1, #0		//int x=0
loop:
	ldr r3,=0xccc	//int delay=(enough amount)
	adds r1,r1,#0x1	//x++
	cmp r1,r3		//compare x with delay
	bne loop		//if not equal go to loop
	bx lr			//if equal break

//Note: If you want to change amout of delay change 0xCCC on line 368

greenled_on://*************************Green Led: ON Function****************//
	ldr r6, =GPIOA_ODR	//Choose: PORTA
	ldr r5, [r6]
	movs r4, LED		//Chose predefined pin: LED
	orrs r5, r5, r4		//Turn on the pin with OR operation
	str r5, [r6]		//Store
	bx lr				//END

greenled_off://***********Green Led: OFF Function****************//
	ldr r6, =GPIOA_ODR	//Choose: PORTA
	ldr r5, [r6]
	movs r4, LED		//Chose predefined pin: LED
	mvns r4, r4
	ands r5, r5, r4		//Turn off the pin with AND operation
	str r5, [r6]		//Store
	bx lr				//END

digit1://************************Segment1 of SSD: ON Function****************//
	ldr r6, =GPIOA_ODR	//Choose: PORTA
	ldr r5, [r6]
	movs r4, SEG1 		//Chose predefined pin: SEG1 for Segment1
	orrs r5, r5, r4		//Turn on the pin with OR operation
	str r5, [r6]

	ldr r6, =GPIOB_ODR	//Choose: PORTB
	ldr r5, [r6]
	movs r4, letS		//Write letter 'S' with predefined value as letS
	mvns r4, r4
	ands r5, r5, r4		//Operate AND for turn on the cells (since SSD is..
	str r5, [r6]		//..common anode
	bx lr				//END

/*Note: Rest of the digitx:ON functions are the same,
*		Example: Change "movs r4, SEG1" for Segment 2 as movs r4, SEG2
*			and  Change "ovs r4, letS" for write something else,
*			example: for digit '0'; "ovs r4, dig0"
*/
digit2://************************Segment2 of SSD: ON Function****************//
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG2
	orrs r5, r5, r4
	str r5, [r6]

	ldr r6, =GPIOB_ODR
	ldr r5, [r6]
	movs r4, letA
	mvns r4, r4
	ands r5, r5, r4
	str r5, [r6]
	bx lr

digit3://************************Segment3 of SSD: ON Function****************//
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG3
	orrs r5, r5, r4
	str r5, [r6]

	ldr r6, =GPIOB_ODR
	ldr r5, [r6]
	movs r4, letF
	mvns r4, r4
	ands r5, r5, r4
	str r5, [r6]
	bx lr

digit4://************************Segment4 of SSD: ON Function****************//
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG4
	orrs r5, r5, r4
	str r5, [r6]
	ldr r6, =GPIOB_ODR
	ldr r5, [r6]
	movs r4, letA
	mvns r4, r4
	ands r5, r5, r4
	str r5, [r6]
	bx lr

clear://************************Segment1 of SSD: ON Function****************//
	ldr r6, =GPIOB_ODR	//Choose: PORTB
	ldr r5, [r6]
	movs r4, 0xFF		//Fill with logic '1' to turn-off all the cells..
	orrs r5, r5, r4		//.. with OR operation
	str r5, [r6]
	bx lr
/*Note: After we write something to cells (ABCDEFG sections of segments),
*		We need to clear to cells for update the next segment
*		This function basicly sends logic '1' to every cell for
*		turning them off -since it's common anode SSD- ...
*/

seg1_off://************************Segment1 of SSD: OFF Function****************//
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG1
	mvns r4, r4
	ands r5, r5, r4
	str r5, [r6]
	bx lr
/*Note: Rest of the digitx:OFF functions are the same,
*		Example: Change "movs r4, SEG1" for Segment 2 as movs r4, SEG2
*/
seg2_off://************************Segment2 of SSD: OFF Function****************//
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG2
	mvns r4, r4
	ands r5, r5, r4
	str r5, [r6]
	bx lr

seg3_off://************************Segment3 of SSD: OFF Function****************//
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG3
	mvns r4, r4
	ands r5, r5, r4
	str r5, [r6]
	bx lr

seg4_off://************************Segment4 of SSD: OFF Function****************//
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	movs r4, SEG4
	mvns r4, r4
	ands r5, r5, r4
	str r5, [r6]
	bx lr

rand://************************Random Number Generator****************//
	push { LR }
	movs r2, #1
	movs r3, #2
	movs r4, #3
	ldr r5, =9999

	bx lr
parser://************************Parser Function****************//
	bx lr

countTOzero://************************Counter Function****************//
	movs r0, #0		//int x = 0
loop_sub:
	ldr r1,=0xccc	//Load generated number
	subs r1,r1,#0x1	//generated number--
	cmp r0,r1		//compare x with number
	bne loop		//if not equal go to loop
	bx lr			//if equal break

/* for(;;); */
	b .

	/* this should never get executed */
	nop
