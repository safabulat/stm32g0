/*
 * name: main.c
 *
 * author: Mahmut Safa BULAT
 *
 * description: This is a state machine example for ELEC334_HW4 problem 4.
 * Every section is properly explained with comments.
 * You can find pin connections at the end of the code (down below the main)
 *
 */

#include "stm32g0xx.h"

#define	dig0	(0x335)	//Predefine '0' digit
#define	dig1	(0x110)	//Predefine '1' digit
#define	dig2	(0x217)	//Predefine '2' digit
#define	dig3	(0x316)	//Predefine '3' digit
#define	dig4	(0x132)	//Predefine '4' digit
#define	letE	(0x227)	//Predefine 'E' letter
#define clear	(0x337)	//Clear the SSD


int mode_buff=0;					//Mode buffer
int button_val=0;					//button value

void EXTI0_1_IRQHandler(void){		//button intterrupt handler
	button_val++;					//Mode menu ++
	EXTI->RPR1 |= (1U << 0);		// EXPI rising edge pending register
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
void reset_SSD(){					//reset function for 4xSSD
	GPIOB->ODR |= (clear);//Clear SSD.
}
void TIM1_BRK_UP_TRG_COM_IRQHandler(void){	//timer handler
	GPIOB->ODR ^= (1U << 6);			//Toggle the primary (Green) LED.
	TIM1->SR	&= ~(1U <<0); 			//clear update status
}
void init_timer1(){					//initialization of the TIM1
	RCC->APBENR2 |= (1U << 11); //enable clock for timer1

	TIM1->CR1	 = 0;
	TIM1->CR1	|= (1<<7);		//enable auto reload
	TIM1->CR1	|= (0<<4);		//direction = up-counter

	TIM1->CNT	 = 0;			//Set default TIM1 values
	TIM1->ARR	 = 1000;		//for 1 second interval
	TIM1->PSC	 = 16000;		//With AutoReloadRegister and PreSCaler.

	TIM1->DIER	|= (1<<0);		//enable update interrupt
	TIM1->CR1	|= (1<<0);		//enable TIM1

	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,2);	//Set priority to 2
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);		//Enable NVIC for TIM1
}
void mode(int mode){				//mode function (state function)

	if(mode == 0){						//No toggling, LED is OFF
		NVIC_DisableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); //Disable NVIC for TIM1
		GPIOB->BRR |= (1U << 6);		//turn off the led
	}
	else if(mode == 1){					//LED Toggling, 1 sec intervals
		NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);		//Enable NVIC for TIM1
		TIM1->ARR	 = 1000;			//Set ARR to 1000 to get 1 sec
	}
	else if(mode == 2){					//LED Toggling, 0.5 sec intervals
		TIM1->CR1	|= (1<<0);			//enable timer
		TIM1->ARR	 = 500;				//Set ARR to 500 to get 0.5 sec
	}
	else if(mode == 3){					//LED Toggling, 0.1 sec intervals
		TIM1->CR1	|= (1<<0);			//enable timer
		TIM1->ARR	 = 100;				//Set ARR to 100 to get 0.1 sec
	}
	else if(mode == 4){					//No toggling, LED is ON
		NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);		//Enable NVIC for TIM1
		GPIOB->ODR |= (1U << 6);		//turn on led
		mode = 0;						//if here, reset the mode back to mode 0.
	}
	else{								//If something else occurs, toggle other LED
		NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);		//Enable NVIC for TIM1
		GPIOB->ODR |= (1U << 7);		//Turn-on Blue LED.
	}
}
void driver_SSD(int Current_mode){	//4xSSD driver function
	if(Current_mode == 0){						//SEG1 = 0;
		GPIOB->ODR |= (clear);
		GPIOA->ODR |= (1U << 15);
		GPIOB->ODR ^= (dig0);
	}
	else if(Current_mode == 1){					//SEG1 = 1;
		GPIOB->ODR |= (clear);
		GPIOA->ODR |= (1U << 15);
		GPIOB->ODR ^= (dig1);
	}
	else if(Current_mode == 2){					//SEG1 = 2;
		GPIOB->ODR |= (clear);
		GPIOA->ODR |= (1U << 15);
		GPIOB->ODR ^= (dig2);
	}
	else if(Current_mode == 3){					//SEG1 = 3;
		GPIOB->ODR |= (clear);
		GPIOA->ODR |= (1U << 15);
		GPIOB->ODR ^= (dig3);
	}
	else if(Current_mode == 4){					//SEG1 = 4;
		GPIOB->ODR |= (clear);
		GPIOA->ODR |= (1U << 15);
		GPIOB->ODR ^= (dig4);
	}
	else{										//SEG1 = E; E for Error
		GPIOB->ODR |= (clear);
		GPIOA->ODR |= (1U << 15);
		GPIOB->ODR ^= (letE);
	}
}
int main(void) {

	//Enable clock for PORT A and PORT B
    RCC->IOPENR |= 0x3;
    //**** Function inits ****//
    init_timer1();
    init_4SSD();
    //**** GPIO Configs ****//
    //INPUT
    GPIOA->MODER &= ~(3U << 2*0);		//Assign PA0 as input
    GPIOA->PUPDR |= (2U << 2*0);		//with pull-down mode.
    //INTERRUPT
    EXTI->RTSR1 	|= (1U << 0);		// EXTI rising trigger selection register
    EXTI->EXTICR[0] |= (0U << 0);		// external interrupt selection register
    EXTI->IMR1 		|= (1U << 0);		// EXTI CPU wake-up with interrupt mask register

    NVIC_SetPriority(EXTI0_1_IRQn,1);	//Set button interrupt priority to 1
    NVIC_EnableIRQ(EXTI0_1_IRQn);		//enable NVIC


    //OUTPUT
    GPIOB->MODER &= ~(0xF000U);	//Allocate Pin 6 and 7
    GPIOB->MODER |= (0x5000U);	//Set as output Pin 6 and 7
    /* LED at PB6 (Green) will be my primary led that i will control,
     * LED at PB7 (Blue) will be my seconder led that it only blink if there is an error  */

    //**** Clear LEDs etc. ****//
    GPIOB->BRR |= (1U << 6);	//Clear PB6
    GPIOB->BRR |= (1U << 7);	//Clear PB7
    GPIOB->ODR |= (clear);		//Clear SSD

   while(1) {
	   mode_buff = button_val;

	   mode(mode_buff%5); 			// %5 is remainder after division (modulo division) for higher values
    								// of mode_buff.

	   driver_SSD(mode_buff%5);		//Show current menu at SSD.
    								//Comment this section if you don't have SSD.

    	/* This function allows me to change my current mode when i press
    	 * the button. If you want to try that function, you can change
    	 * mode_buff%5 to any integer between 0 to 4.
    	 *
    	 * for example, for mode 4; mode(4); will work.
    	 */
    }

    return 0;
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





