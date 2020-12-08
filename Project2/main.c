/*
 * name: main.c
 *
 * author: Mahmut Safa BULAT
 *
 * description: This is a project2 main file for ELEC334 project2.
 * Every section is properly explained with comments.
 * You can find pin connections at the end of the code (down below the main)
 *
 */

#include "stm32g0xx.h"

#define LEDDELAY    1000000

int main(void);
void delay(volatile uint32_t);

#define	dig0	(0x2)	//Predefine '0' digit
#define	dig1	(0x227)	//Predefine '1' digit
#define	dig2	(0x120)	//Predefine '2' digit
#define	dig3	(0x21)	//Predefine '3' digit
#define	dig4	(0x205)	//Predefine '4' digit
#define	dig5	(0x11)	//Predefine '5' digit
#define	dig6	(0x10)	//Predefine '6' digit
#define	dig7	(0x27)	//Predefine '7' digit
#define	dig8	(0x0)	//Predefine '8' digit
#define	dig9	(0x1)	//Predefine '9' digit
#define	letS	(0x11)	//Predefine 'S' letter
#define	letA	(0x4)	//Predefine 'A' letter
#define	letF	(0x114)	//Predefine 'F' letter
#define clear	(0x337)	//Clear the SSD

/*
#define	dig0	(0x335)	//Predefine '0' digit
#define	dig1	(0x110)	//Predefine '1' digit
#define	dig2	(0x217)	//Predefine '2' digit
#define	dig3	(0x316)	//Predefine '3' digit
#define	dig4	(0x132)	//Predefine '4' digit
#define	dig5	(0x326)	//Predefine '5' digit
#define	dig6	(0x327)	//Predefine '6' digit
#define	dig7	(0x310)	//Predefine '7' digit
#define	dig8	(0x337)	//Predefine '8' digit
#define	dig9	(0x336)	//Predefine '9' digit
#define	letS	(0x326)	//Predefine 'S' letter
#define	letA	(0x333)	//Predefine 'A' letter
#define	letF	(0x223)	//Predefine 'F' letter
#define clear	(0x337)	//Clear the SSD
*/

void init_keypad(){	//initialization of the Keypad

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
    GPIOA->MODER |= (0U << 2*12);	//col1     *delete*
    GPIOA->MODER |= (0U << 2*11);	//col2
    GPIOA->MODER |= (0U << 2*6);	//col3
    GPIOA->MODER |= (0U << 2*7);	//col4
}
void init_4SSD(){	//initialization of the 4xSSD

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
	GPIOB->MODER &= ~(3U << 2*5);
	GPIOB->MODER &= ~(3U << 2*4);
	GPIOB->MODER &= ~(3U << 2*2);
	GPIOB->MODER &= ~(3U << 2*1);
	GPIOB->MODER &= ~(3U << 2*0);
	//************************Set pins as output***************//
	GPIOA->MODER |= (1U << 2*15);	//SEG1
	GPIOA->MODER |= (1U << 2*10);	//SEG2
	GPIOA->MODER |= (1U << 2*9);	//SEG3
	GPIOA->MODER |= (1U << 2*8);	//SEG4

	GPIOB->MODER |= (1U << 2*9);	//A
	GPIOB->MODER |= (1U << 2*8);	//C
	GPIOB->MODER |= (1U << 2*5);	//F
	GPIOB->MODER |= (1U << 2*4);	//B
	GPIOB->MODER |= (1U << 2*2);	//D
	GPIOB->MODER |= (1U << 2*1);	//G
	GPIOB->MODER |= (1U << 2*0);	//E
}
void reset_SSD(){
	//************************Reset the SSD***************//

	GPIOB->ODR |= (clear);
}
void font_SSD(){

}
void drive_SSD(){	//Driving the 4xSSD

	//idle state

	GPIOA->ODR |= (1U << 15);	//	SEG1
	GPIOB->ODR &= (letS);	//A
	delay(1000);
	GPIOA->ODR &= (0U << 15);
	GPIOB->ODR |= (clear);

	GPIOA->ODR |= (1U << 8);	//	SEG2
	GPIOB->ODR &= (letA);	//A
	delay(1000);
	GPIOA->ODR &= (0U << 8);
	GPIOB->ODR |= (clear);

	GPIOA->ODR |= (1U << 10);	//	SEG3
	GPIOB->ODR &= (letF);	//A
	delay(1000);
	GPIOA->ODR &= (0U << 10);
	GPIOB->ODR |= (clear);

	GPIOA->ODR |= (1U << 9);	//	SEG4
	GPIOB->ODR &= (letA);	//A
	delay(1000);
	GPIOA->ODR &= (0U << 9);
	GPIOB->ODR |= (clear);

}
uint8_t keypad(){	//Keypad button configurations
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
}
uint8_t pressedKey; //To get the pressed key.

int main(void) {
    /* Enable GPIOC clock */
	//   RCC->IOPENR |= (1U << 0);
    RCC->IOPENR |= 0x3;
    /* Setup PC6 as output */
    init_keypad();
    init_4SSD();

    //PORTB
    GPIOB->MODER &= ~(0xF000U);
    GPIOB->MODER |= (0x5000U);
    /* Turn on LED */
    GPIOB->ODR |= (3U << 6);
    delay(LEDDELAY);
    GPIOB->ODR &= (0U << 6);
    delay(LEDDELAY);

    GPIOB->ODR |= (clear);

    while(1) {
     //   delay(LEDDELAY);
        pressedKey= keypad();


        /* Toggle LED */

        //keypad test
        if(pressedKey=='0'){


        	GPIOA->ODR |= (1U << 15);	//	SEG1
			GPIOB->ODR &= (dig0);	//A
			delay(1000);
			GPIOA->ODR &= (0U << 15);
			GPIOB->ODR |= (clear);

			GPIOB->ODR ^= (1U << 6);

        }
        else if(pressedKey=='1'){
			GPIOB->ODR ^= (1U << 6);

	     	GPIOA->ODR |= (1U << 15);	//	SEG1
				GPIOB->ODR &= (dig1);	//A
				delay(1000);
				GPIOA->ODR &= (0U << 15);
				GPIOB->ODR |= (clear);
		}
        else if(pressedKey=='2'){
			GPIOB->ODR ^= (1U << 6);

	     	GPIOA->ODR |= (1U << 15);	//	SEG1
				GPIOB->ODR &= (dig2);	//A
				delay(1000);
				GPIOA->ODR &= (0U << 15);
				GPIOB->ODR |= (clear);
		}
        else if(pressedKey=='3'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='4'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='5'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='6'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='7'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='8'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='9'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='A'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='B'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='C'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='D'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='E'){
			GPIOB->ODR ^= (1U << 6);
		}
        else if(pressedKey=='F'){
			GPIOB->ODR ^= (1U << 6);
		}
        else{


			drive_SSD();

			GPIOB->ODR ^= (1U << 7);
        }

    }

    return 0;
}

void delay(volatile uint32_t s) {
    for(; s>0; s--);
}

//**********************PIN CONNECTIONs of the BOARD************************//

/*
 * Button; 				PA0		(Don't forget the de-bounce circuit.)
 *
 * 4xSSD;
 * SEG1: 				PA15	(Very Left segment of SSD)
 * SEG2:				PA10
 * SEG3:				PA9
 * SEG4:				PA8		(Very Right segment of SSD)
 *
 * A:					PB9		(eg. A sector of SSD, check datasheet for more.)
 * B:					PB4
 * C:					PB8
 * D:					PB2
 * E:					PB0
 * F:					PB5
 * G:					PB1
 *
 * LED1 (Green LED);	PB6
 * LED2 (Blue LED);		PB7
 */
