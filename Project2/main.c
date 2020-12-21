/*
 * name: main.c
 *
 * author: Mahmut Safa BULAT
 *
 * description: This is a project2 main file for ELEC334 project2;
 * "A Fully Operational Scientific Calculator"
 * Every section is properly explained with comments.
 * You can find pin connections at the end of the code (down below the main)
 */

//********************************Includes*******************************//
#include "stm32g0xx.h"
#include "math.h"
//**************************Includes * END*******************************//
//*****************************Definitions*******************************//
#define clear	(0x3B7)	//Clear the SSD

#define seg1	(1<<9)	//Segment 1 definition
#define seg2	(1<<10)	//Segment 2 definition
#define seg3	(1<<8)	//Segment 3 definition
#define seg4	(1<<15)	//Segment 4 definition
//********************************Definitions * END******************************//

//********************************Global Variables*******************************//
uint32_t digits[4]	={0x0,0x0,0x0,0x0};
uint32_t digits2[4]	={0x0,0x0,0x0,0x0};
uint32_t print[4]	={0x0,0x0,0x0,0x0};
uint32_t print_2[4]	={0x0,0x0,0x0,0x0};
uint32_t val1[4]	={0x0,0x0,0x0,0x0};
uint32_t val1_1[4]	={0x0,0x0,0x0,0x0};

uint32_t val2[4]	={0x0,0x0,0x0,0x0};
uint32_t val2_2[4]	={0x0,0x0,0x0,0x0};

uint32_t operator[3]={0x0,0x0,0x0};
uint32_t operator_2[3]={0x0,0x0,0x0};

uint32_t result[4]	={0x0,0x0,0x0,0x0};
uint32_t result_2[4]={0x0,0x0,0x0,0x0};

double vaL1_show, vaL2_show;
int resulT_show=0;
double resulT_show_float=0;

int stage =0;
int indiss=0;
int pressedKey;
int count =0;
int invalid=0;
int clock_counter=0;
int second;
double pi = 3.141;
//******************************Global Variables * END*************************//

//**********************************Functions**********************************//
void init_timer1(){					//initialization of the TIM1
	RCC->APBENR2 |= (1U << 11); //enable clock for timer1

	TIM1->CR1	 = 0;
	TIM1->CR1	|= (1<<7);		//enable auto reload
	TIM1->CR1	|= (0<<4);		//direction = up-counter

	TIM1->CNT	 = 0;			//Set default TIM1 values
	TIM1->ARR	 = 2000;		//for 1 second interval
	TIM1->PSC	 = 1600;		//With AutoReloadRegister and PreSCaler.

	TIM1->DIER	|= (1<<0);		//enable update interrupt
	TIM1->CR1	|= (1<<0);		//enable TIM1

	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,2);	//Set priority to 2
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);		//Enable NVIC for TIM1

	/* @Name:	init_timer1
	 *
	 * @Brief:	this function configures and enables timer1 with priority of 2.
	 */
}
void delay(volatile uint32_t s) {	//delay function
    for(; s>0; s--);
    /* @Name:	delay
	 *
	 * @Brief:	this function causes desired amount of delay.
	 */
}
void init_keypad(){					//initialization of the Keypad

	/*Keypad Configuration
	* inputs:	pins; PORTA_12/11/6/7	roles; COL_1/2/3/4
	* outputs:	pins; PORTA_5/4/1/0		roles; ROW_1/2/3/4
	*/

    //PORTA
    GPIOA->MODER &= ~(0x3C0FF0FU);	//allocate pins 0 to 12
    //outputs
    GPIOA->MODER |= (1U << 2*5);	//row1
    GPIOA->MODER |= (1U << 2*4);	//row2
    GPIOA->MODER |= (1U << 2*1);	//row3
    GPIOA->MODER |= (1U << 2*0);	//row4
    //inputs
/*	since we allocate pins above, that sends 00' to the
 *	related bit field. 00' also means input mode so we
 *	don't have to do GPIOA->MODER |= (0U << 2*12);	//col1
 *	for inputs. OR operation with 0 is useless anyway.
 *
 *	::: So that means our inputs are ready.
 */

	/* @Name:	init_keypad
	 *
	 * @Brief:	this function activates the pins and set them as outputs or inputs for the keypad module.
	 */

}
void init_4SSD(){					//initialization of the 4xSSD
	/*	For SSD, i will use both port a and port b,
	 * 	that why configuration is a bit mess.
	 */

	//************************Allocating the pins***************//
	GPIOA->MODER &= ~(3U << 2*15);
	GPIOA->MODER &= ~(3U << 2*10);
	GPIOA->MODER &= ~(3U << 2*9);
	GPIOA->MODER &= ~(3U << 2*8);

	GPIOB->MODER &= ~(3U << 2*9);
	GPIOB->MODER &= ~(3U << 2*8);
	GPIOB->MODER &= ~(3U << 2*7);
	GPIOB->MODER &= ~(3U << 2*5);
	GPIOB->MODER &= ~(3U << 2*4);
	GPIOB->MODER &= ~(3U << 2*2);
	GPIOB->MODER &= ~(3U << 2*1);
	GPIOB->MODER &= ~(3U << 2*0);
	//************************Set pins as output***************//
	GPIOA->MODER |= (1U << 2*15);	//SEG4
	GPIOA->MODER |= (1U << 2*10);	//SEG2
	GPIOA->MODER |= (1U << 2*9);	//SEG1
	GPIOA->MODER |= (1U << 2*8);	//SEG3

	GPIOB->MODER |= (1U << 2*9);	//A
	GPIOB->MODER |= (1U << 2*8);	//C
	GPIOB->MODER |= (1U << 2*7);	//.
	GPIOB->MODER |= (1U << 2*5);	//F
	GPIOB->MODER |= (1U << 2*4);	//B
	GPIOB->MODER |= (1U << 2*2);	//D
	GPIOB->MODER |= (1U << 2*1);	//G
	GPIOB->MODER |= (1U << 2*0);	//E

	/* @Name:	init_4SSD
	 *
	 * @Brief:	this function initializaes 4xSSD, enables and configures the pins.
	 */
}
uint8_t keypad(){					//keypad button configurations

	//Set 1 row active(logic 0) and rest deactive(logic 1)
	//and check inputs; if there is button pressed (logic 0)
	//and return a value if pressed.

	//***********************************************ROW 1******************************//
	GPIOA->ODR &= (0U << 5);	//	row1
	GPIOA->ODR |= (1U << 4);	//	row2
	GPIOA->ODR |= (1U << 1);	//	row3
	GPIOA->ODR |= (1U << 0);	//	row4

	delay(10);
	if((~(GPIOA->IDR)) & (1U << 12)){
		return '1';
	}
	if((~(GPIOA->IDR)) & (1U << 11)){
		return '2';
	}
	if((~(GPIOA->IDR)) & (1U << 6)){
		return '3';
	}
	if((~(GPIOA->IDR)) & (1U << 7)){
		return 'A';
	}
	//***********************************************ROW 2******************************//
	GPIOA->ODR &= (0U << 4);	//	row2
	GPIOA->ODR |= (1U << 5);	//	row1
	GPIOA->ODR |= (1U << 1);	//	row3
	GPIOA->ODR |= (1U << 0);	//	row4

	delay(10);
	if((~(GPIOA->IDR)) & (1U << 12)){
		return '4';
	}
	if((~(GPIOA->IDR)) & (1U << 11)){
		return '5';
	}
	if((~(GPIOA->IDR)) & (1U << 6)){
		return '6';
	}
	if((~(GPIOA->IDR)) & (1U << 7)){
		return 'B';
	}
	//***********************************************ROW 3******************************//
	GPIOA->ODR &= (0U << 1);	//	row3
	GPIOA->ODR |= (1U << 5);	//	row1
	GPIOA->ODR |= (1U << 4);	//	row2
	GPIOA->ODR |= (1U << 0);	//	row4

	delay(10);
	if((~(GPIOA->IDR)) & (1U << 12)){
		return '7';
	}
	if((~(GPIOA->IDR)) & (1U << 11)){
		return '8';
	}
	if((~(GPIOA->IDR)) & (1U << 6)){
		return '9';
	}
	if((~(GPIOA->IDR)) & (1U << 7)){
		return 'C';
	}
	//***********************************************ROW 4******************************//
	GPIOA->ODR &= (0U << 0);	//	row4
	GPIOA->ODR |= (1U << 5);	//	row1
	GPIOA->ODR |= (1U << 4);	//	row2
	GPIOA->ODR |= (1U << 1);	//	row3


	delay(10);
	if((~(GPIOA->IDR)) & (1U << 12)){
		return 'E';
	}
	if((~(GPIOA->IDR)) & (1U << 11)){
		return '0';
	}
	if((~(GPIOA->IDR)) & (1U << 6)){
		return 'F';
	}
	if((~(GPIOA->IDR)) & (1U << 7)){
		return 'D';
	}
	//***********************************************ELSE******************************//
	else
		return 'Q'; //return if no button pressed.


		/* @Name:	keypad
		 *
		 * @Brief:	this function gets a value when we press the button on keypad,
		 * and returnes that value to itself. And we can catch the key press like;
		 * 		a = keypad()  => a' captures the key value...
		 */
}
void switcher(int entersth){		//switcher; char(int+48) to HEX

	switch(entersth){
				case '1':
					digits[indiss] = 0x110;
					digits2[indiss] = 1;
					break;
				case '2':
					digits[indiss] = 0x217;
					digits2[indiss] = 2;
					break;
				case '3':
					digits[indiss] = 0x316;
					digits2[indiss] = 3;
					break;
				case '4':
					digits[indiss] = 0x132;
					digits2[indiss] = 4;
					break;
				case '5':
					digits[indiss] = 0x326;
					digits2[indiss] = 5;
					break;
				case '6':
					digits[indiss] = 0x327;
					digits2[indiss] = 6;
					break;
				case '7':
					digits[indiss] = 0x310;
					digits2[indiss] = 7;
					break;
				case '8':
					digits[indiss] = 0x337;
					digits2[indiss] = 8;
					break;
				case '9':
					digits[indiss] = 0x336;
					digits2[indiss] = 9;
					break;
				case '0':
					digits[indiss] = 0x335;
					digits2[indiss] = 0;
					break;
				case 'A':					//return	A
					digits[indiss] = 0x333;
					digits2[indiss] = 'A';
					break;
				case 'B':					//return	b
					digits[indiss] = 0x127;
					digits2[indiss] = 'B';
					break;
				case 'C':					//return	C
					digits[indiss] = 0x225;
					digits2[indiss] = 'C';
					break;
				case 'D':					//return	d
					digits[indiss] = 0x117;
					digits2[indiss] = 'D';
					break;
				case 'E':					//function key E
					digits[indiss] = 0x227;
					digits2[indiss] = 'E';
					break;
				case 'F':					//enter, on/off key
					digits[indiss] = 0x223;
					digits2[indiss] = 'F';
					break;
				default:
					digits[indiss] = 0x0;
			}

	/* @Name:	switcher
	 *
	 * @Brief:	this function switches chars values to hex values (and char values again),
	 * theese hex values are for SSD. For example if i send 'E' to the SSD it prints nothing or junk, but if i send 0x227,
	 * it will show E on SSD.
	 */

}
void capture_key(int keycapture){	//capture (switched) key
	//Get the key if != Q or F........................................
	if((keycapture != 'Q') && (indiss <= 4) && (keycapture != 'F')){

			switcher(keycapture);//send the char to the switcher to get hex

			//Not: See the description below to understand what is going on here.

			if(indiss == 0){	//if only 1 digit : no shifting
				print[3]= 0x0;
				print[2]= 0x0;
				print[1]= 0x0;
				print[0]= digits[indiss];

				print_2[3]= 0x0;
				print_2[2]= 0x0;
				print_2[1]= 0x0;
				print_2[0]= digits2[indiss];
			}
			if(indiss == 1){	//if 2 digit : 1 shifting
				print[3]= 0x0;
				print[2]= 0x0;
				print[1]= digits[indiss-1];
				print[0]= digits[indiss];

				print_2[3]= 0x0;
				print_2[2]= 0x0;
				print_2[1]= digits2[indiss-1];
				print_2[0]= digits2[indiss];

			}
			if(indiss == 2){	//if 3 digit : 2 shifting
				print[3]= 0x0;
				print[2]= digits[indiss-2];
				print[1]= digits[indiss-1];
				print[0]= digits[indiss];

				print_2[3]= 0x0;
				print_2[2]= digits2[indiss-2];
				print_2[1]= digits2[indiss-1];
				print_2[0]= digits2[indiss];

			}
			if(indiss == 3){	//if 4 digit : 3 shifting
				print[3]= digits[indiss-3];
				print[2]= digits[indiss-2];
				print[1]= digits[indiss-1];
				print[0]= digits[indiss];

				print_2[3]= digits2[indiss-3];
				print_2[2]= digits2[indiss-2];
				print_2[1]= digits2[indiss-1];
				print_2[0]= digits2[indiss];
			}
			indiss++;
			//Reset the timeout value if something is pressed
			second=0;
			clock_counter=0;

			if(indiss >= 4){
				//reset
				indiss=0;
			}
		}

	else if(keycapture == 'F'){
		switcher(keycapture);
		//Reset the timeout value if  F pressed
		second=0;
		clock_counter=0;
	}

	/* @Name:	capture_key
	 * @Brief:	The mechanism abowe is basicly makes the shitfing (the digits) .
	 * There is no point of commenting every line, it does for example;
	 * 					for	digit1=3 and digit0=2 if i press 3
	 * 					digits become; d0 = 3, d1=2, d2=3
	 */
}
void segment1(uint32_t aa){			//drive : segment 1

	GPIOB->ODR |= (clear);	//clear SSD
	GPIOA->ODR |= (seg1);	//choose segment
	GPIOB->ODR ^= (aa);		//print argument
	GPIOA->ODR &= !(seg1);	//turn off the segment

	//Other segmentX functions are same with this function.

	 /* @Name:	segment1
	 *
	 * @Brief:	this function drives the first segment
	 * of the 4xSSD
	 */
}
void segment2(uint32_t bb){			//drive : segment 2

	GPIOB->ODR |= (clear);
	GPIOA->ODR |= (seg2);
	GPIOB->ODR ^= (bb);
	GPIOA->ODR &= !(seg2);

	 /* @Name:	segment2
	 *
	 * @Brief:	this function drives the second segment
	 * of the 4xSSD
	 */
}
void segment3(uint32_t cc){			//drive : segment 3

	GPIOB->ODR |= (clear);
	GPIOA->ODR |= (seg3);
	GPIOB->ODR ^= (cc);
	GPIOA->ODR &= !(seg3);

	 /* @Name:	segment3
	 *
	 * @Brief:	this function drives the third segment
	 * of the 4xSSD
	 */
}
void segment4(uint32_t dd){			//drive : segment 4

	GPIOB->ODR |= (clear);
	GPIOA->ODR |= (seg4);
	GPIOB->ODR ^= (dd);
	GPIOA->ODR &= !(seg4);

	 /* @Name:	segment4
	 *
	 * @Brief:	this function drives the last segment
	 * of the 4xSSD
	 */
}
void printer(uint32_t segm1, uint32_t segm2, uint32_t segm3, uint32_t segm4){	//4xSSD driver function

	//call all the segment functions and write something on SSD
	segment1(segm1);
	segment2(segm2);
	segment3(segm3);
	segment4(segm4);

	 /* @Name:	printer
	 *
	 * @Brief:	this function drives all the segments
	 * of the 4xSSD by sending desired HEX codes to the SSD.
	 */
}
void idle_state(){					//idle state for calculator (stand by)
	uint32_t idle[4];//it should show my ID.
	idle[0]= 0x110;//1
	idle[1]= 0x326;//5
	idle[2]= 0x132;//4
	idle[3]= 0x110;//1
	printer(idle[0],idle[1],idle[2],idle[3]);	//writes 1451 for my id (141024051)

	 /* @Name:	idle_state
	 *
	 * @Brief:	this function is stand-by phase of the calculator.
	 * It shows desired 4 digit on the SSD at start. You can activate calculator by pressing F button.
	 * The calculator will return to this stage if no button is pressed for 10 seconds.
	 */
}
_Bool isInteger(double val){		//Little operation for detecting float numbers.
	int truncated = (int)val;
	return (val == truncated);
}
void calculator_functions(){		//(some) calculator functions

	//deparser.............................................
	result_2[0]= (uint32_t)(resulT_show %10);
	result_2[1]= (uint32_t)((resulT_show/10)%10);
	result_2[2]= (uint32_t)((resulT_show/100)%10);
	result_2[3]= (uint32_t)((resulT_show/1000)%10);
	//...................................................

	//check negatif......................................
	if(resulT_show < 0){	//if negatif add: -
		result_2[0]= (uint32_t)(abs(resulT_show)%10);
		result_2[1]= (uint32_t)(((abs(resulT_show))/10)%10);
		result_2[2]= (uint32_t)(((abs(resulT_show))/100)%10);
		result_2[3]= (uint32_t)(((abs(resulT_show))/1000)%10);
	}
	//...................................................

	//deswitcher.........................................
	NVIC_DisableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);		//Enable NVIC for TIM1
	digits[0]=0;
	indiss=0;

	capture_key((char)(result_2[0]+48));
	result[0]=digits[0];

	capture_key((char)(result_2[1]+48));
	result[1]=digits[1];

	capture_key((char)(result_2[2]+48));
	result[2]=digits[2];

	capture_key((char)(result_2[3]+48));
	result[3]=digits[3];

	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);		//Enable NVIC for TIM1
	//...................................................

	//check overflow.....................................
	if((resulT_show > 9999) | (resulT_show < -999)){	//overflow as: 0uFL

		result[0]= 0x25;	//0x25  for L
		result[1]= 0x223;	//0x223 for F
		result[2]= 0x105;	//0x105 for u
		result[3]= 0x335;	//0x335 for 0
	}
	//...................................................

	//check invalid .....................................
	if((invalid != 0) ||
			(((operator_2[0] == 'D') && (operator_2[1] == 0x0) && (operator_2[2] == 0x0)) && (vaL2_show == 0) )  ){
		//invalid as: 1nLd

		result[0]= 0x117;	//0x117 for d
		result[1]= 0x25;	//0x25  for L
		result[2]= 0x103;	//0x103 for n
		result[3]= 0x110;	//0x110 for 1
	}
	//...................................................

	//if float, add . .................................
	if(isInteger(resulT_show_float) == 0){	//if negatif add: -


		if( (result[2] != 0x335) && (result[3] != 0x335) ){ //if number like 432.123
			//shift the digits, put dot at 2th digit


			if(resulT_show_float < 0){
				//result[2] = 0 with dot, result[3] is for sign. (if negative)
				result[3]=0x2;
				//we cant print the float part of the number,
				//if the number is like -432,32..
			}
			else{
				//result[3] = 0 with dot, no need to sign digit

				result_2[0]= ((uint32_t)(resulT_show_float*10))%10;

				capture_key((char)(result_2[0]+48));
				result[0]=digits[0];

				capture_key((char)(result_2[1]+48));
				result[1]=digits[0] + (0x80);

				capture_key((char)(result_2[2]+48));
				result[2]=digits[1];

				capture_key((char)(result_2[3]+48));
				result[3]=digits[2];
			}
		}
		else if(  (result[1] != 0x335) && (result[2] != 0x335) && (result[3] != 0x335) ){ //if number like 32.123
			//shift the digits, put dot at 3th digit


			if(resulT_show_float < 0){
				//result[2] = 0 with dot, result[3] is for sign. (if negative)
				result[3]= 0x2;
				result_2[0]= ((uint32_t)(resulT_show_float*10))%10;

				capture_key((char)(result_2[0]+48));
				result[0]=digits[0];

				capture_key((char)(result_2[1]+48));
				result[1]=digits[0] + (0x80);

				capture_key((char)(result_2[2]+48));
				result[2]=digits[1];

			}
			else{
				//result[3] = 0 with dot, no need to sign digit
				result[3]= result[1];
				result[2]= result[0] + (0x80);
				result_2[1]= ((uint32_t)(resulT_show_float*10))%10;
				result_2[0]= ((uint32_t)(resulT_show_float*100))%100;

				capture_key((char)(result_2[0]+48));
				result[0]=digits[0];

				capture_key((char)(result_2[1]+48));
				result[1]=digits[1];

				capture_key((char)(result_2[2]+48));
				result[2]=digits[2];
			}
		}
		else if( (result[0] != 0x335) && (result[1] != 0x335) && (result[2] != 0x335) && (result[3] != 0x335) ){
			//if number like 2.123
			//shift the digits, put dot at 4th digit


			if(resulT_show_float < 0){
				//result[2] = 0 with dot, result[3] is for sign. (if negative)
				result[3]=0x2;
				result_2[1]= ((uint32_t)(resulT_show_float*10))%10;
				result_2[0]= (((uint32_t)(resulT_show_float*100))%100) - (result_2[1]*10);

				capture_key((char)(result_2[0]+48));
				result[0]=digits[0];

				capture_key((char)(result_2[1]+48));
				result[1]=digits[1];

				capture_key((char)(result_2[2]+48));
				result[2]=digits[2] + (0x80);
			}
			else{
				//result[3] = 0 with dot, no need to sign digit
				result[3]= result[0] + (0x80);
				result_2[2]= ((uint32_t)(resulT_show_float*10))%10;
				result_2[1]= (((uint32_t)(resulT_show_float*100))%100) - (result_2[2]*10);
				result_2[0]= (((uint32_t)(resulT_show_float*1000))%1000) - (((result_2[2]*10) + result_2[1])*10);

				capture_key((char)(result_2[0]+48));
				result[0]=digits[0];

				capture_key((char)(result_2[1]+48));
				result[1]=digits[1];

				capture_key((char)(result_2[2]+48));
				result[2]=digits[2];

				capture_key((char)(result_2[3]+48));
				result[3]=digits[3] + (0x80);
			}
		}
		//0x335 is HEX code for SSD to print 0
		else if( (result[0] == 0x335) && (result[1] == 0x335) && (result[2] == 0x335) && (result[3] == 0x335) ){ //if number like 0.123

			//rest of the float is must be placed..
			if(resulT_show_float < 0){
				//result[2] = 0 with dot, result[3] is for sign. (if negative)
				result[3]=0x2;
				result[2]= 0x335 + (0x80);
				result_2[1]= ((uint32_t)(resulT_show_float*10))%10;
				result_2[0]= (((uint32_t)(resulT_show_float*100))%100) - (result_2[1]*10);

				capture_key((char)(result_2[0]+48));
				result[0]=digits[0];

				capture_key((char)(result_2[1]+48));
				result[1]=digits[1];

			}
			else{
				//result[3] = 0 with dot, no need to sign digit
				result[3]= 0x335 + (0x80);
				result_2[2]= ((uint32_t)(resulT_show_float*10))%10;
				result_2[1]= (((uint32_t)(resulT_show_float*100))%100) - (result_2[2]*10);
				result_2[0]= (((uint32_t)(resulT_show_float*1000))%1000) - (((result_2[2]*10) + result_2[1])*10);

				capture_key((char)(result_2[0]+48));
				result[0]=digits[0];

				capture_key((char)(result_2[1]+48));
				result[1]=digits[1];

				capture_key((char)(result_2[2]+48));
				result[2]=digits[2];
			}

		}
		else{//there is no floating number to print : example; 1234.234
			resulT_show_float=0;
		}

	}
	//...................................................

	//if negatif, add - .................................
	if(resulT_show < 0){	//if negatif add: -

		result[3]= 0x2;
	}
	//...................................................

	 /* @Name:	calculator_functions
	 *
	 * @Brief:	this function operates some other functions for calculator.
	 *
	 * DeParser:	It deparses digit (number) and makes an integer.
	 * For example i have example[4] = {1,2,3,4} , it becaomes a_int = 1234
	 *
	 * Check Negatif: It detects if my number is negatif.
	 * If my number is negatif put(-)sign*
	 *
	 * Check Overflow:	It detects if there is an overflow;
	 * Since i use 4xSSD, my interval is only -999 to 9999.
	 * Anything beyond; returnes as overflow.
	 *
	 * Check Invalid:	It detects if there is an invalid operation;
	 * such as division with zero ( example 15/0 ) if so, returnes as invalid.
	 *
	 * Check Float:	It detects if the number is float or integer,
	 * if it is float; place dot at proper place.
	 *
	 * Put (-) sign:	If the number detected as negatif, put (-) minus sign.
	 */
}
void calculator(){					//the Calculator

	if(count == 0){//Stay at IDLE state............................................
		idle_state();			//call idle state function.
		if(pressedKey == 'F'){	//if F pressed;
			count++;			//count++
			stage=1;			//go to calculator stage
			delay(1000000);		//delay some
		}
	}
	if(count == 1){//Enter Calculator mode.........................................

		//Get value 1...................................................
		if(stage==1){
			GPIOB->ODR |= (clear);//clear SSD

			val1[3]=print[3];//get hex values for SSD
			val1[2]=print[2];
			val1[1]=print[1];
			val1[0]=print[0];

			val1_1[3]=print_2[3];//get real values for computation
			val1_1[2]=print_2[2];
			val1_1[1]=print_2[1];
			val1_1[0]=print_2[0];

			printer(val1[0],val1[1],val1[2],val1[3]);	//print : value 1

			if(pressedKey == 'F'){	//if F pressed;
				stage++;		  	//go to next stage
				delay(1000000);

				digits[3] =0x0;		//clean cache
				digits[2] =0x0;
				digits[1] =0x0;
				digits[0] =0x0;

				digits2[3] =0x0;	//clean cache
				digits2[2] =0x0;
				digits2[1] =0x0;
				digits2[0] =0x0;
			}
		}
		//..............................................................

		//Get operators.................................................
		else if(stage==2){
			GPIOB->ODR |= (clear);//clear SSD

			operator[2]=print[2];//get hex values for SSD
			operator[1]=print[1];
			operator[0]=print[0];

			operator_2[2]=print_2[2];//get real values for computation
			operator_2[1]=print_2[1];
			operator_2[0]=print_2[0];

			printer(operator[0],operator[1],operator[2],0x0);	//print : operators
			if(pressedKey == 'F'){	//if F pressed;
				stage++;		  	//go to next stage
				delay(1000000);

				digits[3] =0x0;		//clean cache
				digits[2] =0x0;
				digits[1] =0x0;
				digits[0] =0x0;

				digits2[3] =0x0;	//clean cache
				digits2[2] =0x0;
				digits2[1] =0x0;
				digits2[0] =0x0;
			}

		}
		//..............................................................

		//Get value 2...................................................
		else if(stage==3){
			GPIOB->ODR |= (clear);//clear SSD

			val2[3]=print[3];//get hex values for SSD
			val2[2]=print[2];
			val2[1]=print[1];
			val2[0]=print[0];

			val2_2[3]=print_2[3];//get real values for computation
			val2_2[2]=print_2[2];
			val2_2[1]=print_2[1];
			val2_2[0]=print_2[0];

			printer(val2[0],val2[1],val2[2],val2[3]);	//print : value 2
			if(pressedKey == 'F'){	//if F pressed;
				stage++;		  	//go to next stage
				delay(1000000);

				digits[3] =0x0;		//clean cache
				digits[2] =0x0;
				digits[1] =0x0;
				digits[0] =0x0;

				digits2[3] =0x0;	//clean cache
				digits2[2] =0x0;
				digits2[1] =0x0;
				digits2[0] =0x0;
			}
		}
		//..............................................................

		//Compute stage.................................................
		else if(stage==4){

			//clear SSD..........................................
			GPIOB->ODR |= (clear);

			//if A on values : use previous answer................
			if(val1_1[0] == 'A'){
				val1_1[0]=result_2[0];
				val1_1[1]=result_2[1];
				val1_1[2]=result_2[2];
				val1_1[3]=result_2[3];
			}
			//...................................................

			//Transport numbers to int from digits...............
			vaL1_show= (val1_1[3]*1000)+(val1_1[2]*100)+(val1_1[1]*10)+(val1_1[0]);
			//...................................................

			//if EEE on values : set is as pi number.............
			if((val1_1[0] == 'E') && (val1_1[1] == 'E') && (val1_1[2] == 'E')){
				vaL1_show= pi;
			}
			//...................................................

			//if A on values : use previus answer................
			if(val2_2[0] == 'A'){
				val2_2[0]=result_2[0];
				val2_2[1]=result_2[1];
				val2_2[2]=result_2[2];
				val2_2[3]=result_2[3];
			}
			//...................................................

			//Transport numbers to int from digits...............
			vaL2_show= (val2_2[3]*1000)+(val2_2[2]*100)+(val2_2[1]*10)+(val2_2[0]);
			//...................................................

			//if EEE on values : set is as pi number.............
			if((val2_2[0] == 'E') && (val2_2[1] == 'E') && (val2_2[2] == 'E')){
				vaL2_show= pi;
			}
			//...................................................
			delay(100);
			//if != E and ??? : basic mode.......................
			if((operator[0] != 0x227) && (operator[1] == 0x0) && (operator[2] == 0x0)){
				if(operator_2[0] == 'A'){					//if A : operate addition
					//result =val1+val2;
					delay(100);
					resulT_show=(int)(vaL1_show+vaL2_show);
				}
				if(operator_2[0] == 'B'){					//if B : operate subtraction
					//result =val1-val2;
					delay(100);
					resulT_show=(int)(vaL1_show-vaL2_show);
				}
				if(operator_2[0] == 'C'){					//if C : operate multiplication
					//result =val1*val2;
					delay(100);
					resulT_show=(int)(vaL1_show*vaL2_show);
				}
				if(operator_2[0] == 'D'){					//if D : operate divison
					//result =val1/val2;
					delay(100);
					resulT_show=(int)(vaL1_show/vaL2_show);
				}
			}
			//...................................................

			//if E__ and ?? : scientific mode....................
			else if((operator[0] != 0x227) && (operator[1] == 0x227) && (operator[2] == 0x0)){
				if(operator_2[0] == 'A'){					//if A : operate log
					//result =val1 * log(val2);
					delay(100);
					resulT_show=(int)(vaL1_show * log10(vaL2_show));
				}
				if(operator_2[0] == 'B'){					//if B : operate ln
					//result =val1 * ln(val2);
					delay(100);
					resulT_show=(int)(vaL1_show * log(vaL2_show));
				}
				if(operator_2[0] == 'C'){					//if C : operate sqrt
					//result =val1 * sqrt(val2);
					delay(100);
					resulT_show=(int)(vaL1_show * sqrt(vaL2_show));
				}
				if(operator_2[0] == 'D'){					//if D : operate pow
					//result =val1 ^ val2;
					delay(100);
					resulT_show=(int)(pow(vaL1_show, vaL2_show));
				}
			}
			//...................................................

			//if EE_ and ? : trigonometric mode..................
			else if((operator[0] != 0x227) && (operator[1] == 0x227) && (operator[2] == 0x227)){

				if(operator_2[0] == 'A'){					//if A : operate sin
					//result =val1 * sin(val2);
					delay(100);
					resulT_show_float=(vaL1_show * sin(vaL2_show));
					resulT_show=(int )(resulT_show_float); //get integer part of the float and send it
				}
				if(operator_2[0] == 'B'){					//if B : operate cos
					//result =val1 * cos(val2);
					delay(100);
					resulT_show_float=(vaL1_show * cos(vaL2_show));
					resulT_show=(int )(resulT_show_float);
				}
				if(operator_2[0] == 'C'){					//if C : operate tan
					//result =val1 * tan(val2);
					delay(100);
					resulT_show_float=(vaL1_show * tan(vaL2_show));
					resulT_show=(int )(resulT_show_float);
				}
				if(operator_2[0] == 'D'){					//if D : operate cot
					//result =val1 * cot(val2); since there is no cot(x) function and 1/tan(x) = cot(x)
					delay(100);
					resulT_show_float=(vaL1_show * (1/tan(vaL2_show)));
					resulT_show=(int )(resulT_show_float);
				}
			}
			//...................................................

			//clean operators....................................
			operator[0]=0x0;
			operator[1]=0x0;
			operator[2]=0x0;
			delay(100);
			//...................................................

			//call other functions...............................
			calculator_functions();
			delay(100);
			//...................................................

			//cleaning unnecessary values........................
			digits[0]=0x0;
			digits[1]=0x0;
			digits[2]=0x0;
			digits[3]=0x0;

			print[0]=0x0;
			print[1]=0x0;
			print[2]=0x0;
			print[3]=0x0;

			delay(100);
			//...................................................
			stage++;
		}
		//..............................................................

		//Print Stage...................................................
		else if(stage==5){
			printer(result[0],result[1],result[2],result[3]); //print results.

			if(pressedKey == 'F'){ //if press F : stage++
				stage++;
				delay(1000000);
			}
		}
		//..............................................................

		//Cleaning, preparetion stage...................................
		else if(stage>5){
			stage=1;						//Go to the start.

			GPIOB->ODR |= (clear);			//Clear SSD.



			digits[3] =0x0;					//Clean cache.
			digits[2] =0x0;
			digits[1] =0x0;
			digits[0] =0x0;

			digits2[3] =0x0;				//Clean cache.
			digits2[2] =0x0;
			digits2[1] =0x0;
			digits2[0] =0x0;

			delay(1000000);					//Delay some.
		}
		//..............................................................
	}

	 /* @Name:	calculator
	 *
	 * @Brief:	this function is calculator itself. It operates the following operations;
	 *
	 * Mode: Basic				Operator:	0x0 0x0 ?
	 * 		-addition								A
	 * 		-substaction							B
	 * 		-multipication							C
	 * 		-division								D
	 *
	 * Mode: Scientific			Operator:	0x0 E 	?
	 * 		-log									A
	 * 		-ln										B
	 * 		-sqrt									C
	 * 		-power									D
	 *
	 * Mode: Trigonometric		Operator:	E 	E 	?
	 * 		-sin									A
	 * 		-cos									B
	 * 		-tan									C
	 * 		-cot									D
	 *
	 * -You can compute negatif numbers, float number as well
	 * -You can replace the number with pi with EEE
	 * -Calculator will go to the idle state if no button is pressed for 10seconds
	 * -You can use previous resuld as a new value by 'A'
	 */
}
//*******************************Functions * END*******************************//

//*****************************Interrupts and main****************************//
void TIM1_BRK_UP_TRG_COM_IRQHandler(void){	//timer handler
	pressedKey= keypad();		//get pressed key
	capture_key(pressedKey);	//send pressed key
	clock_counter++;
	if(clock_counter == 5){		//for 10sec timeout
		clock_counter=0;
		second++;
		if(second==10){			//if 10sec has passed;
			second=0;
			count=0;
		}
	}
	TIM1->SR	&= ~(1U <<0); 			//clear update status
}
int main(void) {							//main
	//Enable clock for PORT A and PORT B
	RCC->IOPENR |= 0x3;
	//**** Function inits ****//
	init_timer1();
	init_keypad();
	init_4SSD();
	//**** GPIO Configs ****//

	//OUTPUT
	GPIOA->MODER &= ~(3U << 2*6);	//Allocate Pin 6
	GPIOB->MODER |=  (1U << 2*6);	//Set Pin 6 as output
	/*
	 * LED at PB6 (Green) will be my test LED
	 */

	//**** Clear LEDs etc. ****//
	GPIOB->BRR |= (1U << 6);	//Clear PB6
	GPIOB->ODR |= (clear);		//Clear SSD

    /* Toggle LEDs on start */
    GPIOB->ODR |= (1U << 6);	//turn on test leds
    delay(1000000);				//delay for a second
    GPIOB->ODR &= (0U << 6);	//turn off test leds
    delay(1000000);				//delay for a second

    //the calculator is ready to use now;
    while(1) {
    	calculator();
    //	float_parser(vaL1_show);
    }
    return 0;
}
//*************************Interrupts and main * END*************************//


//**********************PIN CONNECTIONs of the BOARD************************//

/*
 * Keypad;.....................................................................
 *  					(Don't forget the de-bounce buttons.)
 *
 *  Col1				PA7
 *  Col2				PA6
 *  Col3				PA11
 *  Col4				PA12
 *
 *  Row1				PA5
 *  Row2				PA4
 *  Row3				PA1
 *  Row4				PA0
 * ............................................................................
 * 4xSSD;......................................................................
 * SEG4: 				PA15	(Very Left segment of SSD)
 * SEG3:				PA8
 * SEG2:				PA10
 * SEG1:				PA9		(Very Right segment of SSD)
 *
 * A:					PB9		(eg. A sector of SSD, check datasheet for more.)
 * B:					PB4
 * C:					PB8
 * D:					PB2
 * E:					PB0
 * F:					PB5
 * G:					PB1
 * DOT:					PB7
 * ............................................................................
 * Test LED(s);................................................................
 * (Green LED);			PB6
 * ............................................................................
 */
