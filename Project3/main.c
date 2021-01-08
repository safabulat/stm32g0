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
#include "stm32g0_i2c-lcd.h"
#include "stdio.h"
//**************************Includes * END*******************************//
//*****************************Definitions*******************************//
#define eeprom_1_address	0x50
#define eeprom_2_address	0x54
#define lcd_address			0x27

#define secdelay 1000000

//********************************Definitions * END******************************//

//********************************Global Variables*******************************//
int state,start,idle =0;

int pressedKey;
int count =0;
int clock_counter=0;
int second,mecond;
uint8_t a[10];
uint32_t inputCaptureVal=0x0;

uint32_t pwmvalue;
uint32_t pwmfreq=160;

uint8_t track;
uint16_t regAddr1,regAddr2=0x0;
uint16_t regAddrEnd=0xFFFF;
uint16_t track1End=0xFFFF;
uint16_t track2End=0x7FFF;
//******************************Global Variables * END*************************//

//**********************************Functions**********************************//
void delay(volatile uint32_t s) {	//delay function
    for(; s>0; s--);
    /* @Name:	delay
	 *
	 * @Brief:	this function causes desired amount of delay.
	 */
}
void init_timer1(){											//initialization of the TIM1
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
void init_timer2(){											//initialization of the TIM2 for PWM
	RCC->APBENR1 |= (1U << 0); 		//enable clock for timer2
	RCC->IOPENR  |= (1U << 1);		//enable GPIOB for PWM out at PB3


	TIM2->CR1	 = 0;				//reset control register1
	TIM2->CCMR1	 = 0;				//reset capture/compare register

	TIM2->CR1	|= (1U<<4);			//direction = down-counter

	TIM2->CNT	 = 0;				//Set default TIM2 values
	TIM2->ARR	 = pwmfreq;			//for 25600 Hz
	TIM2->PSC	 = 500;			//With AutoReloadRegister and PreSCaler.
	TIM2->CCR2	 = pwmfreq/2;		//for %50 duty cycle
	//PWM configuration for TIM2_CH2
	TIM2->CCMR1 &= ~(0x7U << 12);			//reset oc2m
    TIM2->CCMR1 |= (0x6U << 12);			//oc2m; pwm mode 1 for channel2

    TIM2->CR1	|= (1U<<7);					//enable auto reload
    TIM2->EGR 	|= (1<<0);      			//update generation

    TIM2->CCMR1 |= (1U << 11);				//enable output compare2 preload
    TIM2->CCER 	|= (1U << 4);        		//enable capture/compare2 output

    GPIOB->MODER &= ~(3U << 2*3);      		//pb3
    GPIOB->MODER |= (2U << 2*3);        	//alternate mode
//    GPIOB->OSPEEDR |= (3U << 2*3);        	//high precision on change
    GPIOB->AFR[0] &= ~(0xFU << 4*3);   		//af4 for = TIM2_CH2
    GPIOB->AFR[0] |= (2U << 4*3);        	// AFSEL5 -> AF2 [0010]

//	TIM2->TISEL &= ~(0xFU << 8);

	TIM2->DIER	|= (1<<0);		//enable update interrupt
	TIM2->CR1	|= (1<<0);		//enable TIM2

	NVIC_SetPriority(TIM2_IRQn,2);	//Set priority to 2
	NVIC_EnableIRQ(TIM2_IRQn);		//Enable NVIC for TIM1
	/* @Name:	init_timer2
	 *
	 * @Brief:	this function configures and enables timer.
	 */
}
void init_timer3(){											//initialization of the TIM3 for ADC
	RCC->APBENR1 |= (1U << 1); //enable clock for timer3

	TIM3->CR1	 = 0;
	TIM3->CR1	|= (1<<7);		//enable auto reload
	TIM3->CR1	|= (1<<4);		//direction = down-counter

	TIM3->CNT	 = 0;		//Set default TIM3 values
	TIM3->ARR	 = 125;		//for 25600 Hz
	TIM3->PSC	 = 5;		//With AutoReloadRegister and PreSCaler.

	TIM3->CR2 |= (2U << 4);		//MMS; Update

	TIM3->CR1	|= (1<<0);		//enable TIM3
	/* @Name:	init_timer3
	 *
	 * @Brief:	this function configures and enables timer.
	 */
}
void init_i2c(){											//initialization of the I2C1

	//Enable GPIOB
	RCC->IOPENR |= (1U << 1);

	//Setup PB6 as AF6
	GPIOB->MODER &= ~(3U << 2*6);
	GPIOB->MODER |= (2 << 2*6);
	GPIOB->OTYPER |= (1U << 6);
	GPIOB->PUPDR |= (1<< 2*6);  //Pull up for PB6
	//Chose AF6 from mux
	GPIOB->AFR[0] &= ~(0xFU << 4*6);
	GPIOB->AFR[0] |= (6 << 4*6);
	//Setup PB7 as AF6
	GPIOB->MODER &= ~(3U << 2*7);
	GPIOB->MODER |= (2U << 2*7);
	GPIOB->OTYPER |= (1U << 7);
	GPIOB->PUPDR |= (1<< 2*7);  //Pull up for PB7
	//Chose AF6 from mux
	GPIOB->AFR[0] &= ~(0xFU << 4*7);
	GPIOB->AFR[0] |= (6 << 4*7);
	//Enable i2c1
	RCC->APBENR1 |= (1U << 21);

	I2C1->CR1 =0;
	I2C1->CR1 |= (1U << 7); //ERRI

	I2C1->TIMINGR |= (3 << 28);		//PRESCALAR
	I2C1->TIMINGR |= (0x13 << 0);	//SCLL
	I2C1->TIMINGR |= (0xF << 8);	//SCLH
	I2C1->TIMINGR |= (0x2 << 16);	//SDADEL
	I2C1->TIMINGR |= (0x4) << 20;	//SCLDEL

	I2C1->CR1 |= (1U << 0);			//PE

	NVIC_SetPriority(I2C1_IRQn,2);	//Set priority to 2
	NVIC_EnableIRQ(I2C1_IRQn);		//Enable NVIC for TIM1

	/* @Name:	init_i2c
		 *
		 * @Brief:	this function configures and enables i2c bus for
		 * PB6 (for SCL) and PB7 (for SDA) pins with priority of 2.
		 */
}
void init_adc(){

	RCC->APBENR2 |= (1U << 20); 	//enable rcc for adc
	RCC->IOPENR |= (1U << 1);		//enable GPIOB
	//PB1 pin for adc in analog mode (by default)

	ADC1->CR	 = 0;				//reset adc cr
	ADC1->CFGR1  = 0;				//reset adc cfgr1

	ADC1->CR |= (1U << 28);			//Enable adc voltage regulator
	delay(500);						//delay >20 uS

	//enable calibration, wait until completion
	ADC1->CR |= (1U << 31);			//calibration enable
	while(!(ADC1->ISR & (1 << 11)));//Wait until EOCAL=1.

	//enable end of cal. or sequence interrupts
	ADC1->IER |= (1U << 2); //end of conversion sequence interrupt
//	ADC1->IER |= (1U << 11); //end of calibration interrupt

	//select resolution [conf. bit sample (6,8,10,12)]
	ADC1->CFGR1 |= (2U << 3);// ; 8bit

	//conf. single/continuous;
	ADC1->CFGR1 &= ~(1U << 13);//cont=0;
	ADC1->CFGR1 &= ~(1U << 16);//discen =0; single

	//select sampling time from SMPR
	ADC1->SMPR |= (0 << 0);//SMP1
	ADC1->SMPR |= (1U << 4);//SMP2

	//select tim3 trgo
	ADC1->CFGR1 |= (3U << 6); //TGRO (extsel); 0xb011=3U for TIM3_TRGO
	ADC1->CFGR1 |= (1U << 10); //Choose detect at rising edge (exten); 01

	//enable channels (for the ANx pins)
	ADC1->CFGR1 |= (9U << 26);//analog input channel 9; PB1
	ADC1->CHSELR |= (1U << 9);//analog input channel 9; PB1

	//Clear the ADRDY bit in ADC_ISR register by programming this bit to 1.
	ADC1->ISR |= (1U << 0);

	//enable adc and wait until it is ready
	ADC1->CR |= (1U << 0);
	while(!(ADC1->ISR & (1U << 0)));

	NVIC_SetPriority(ADC1_IRQn,2);	//Set priority to 2
	NVIC_EnableIRQ(ADC1_IRQn);		//Enable NVIC for TIM1

	//Start conversion
	ADC1->CR |= (1U << 2);
}
void write_I2C(uint8_t devAddr,uint8_t* data, int Size){	//write function for i2c1 (no address)
	//Send address and register to read
	I2C1->CR2 =0;									//clear register
	I2C1->CR2 |= (uint32_t)(devAddr << 1);			//Send slave address
	I2C1->CR2 |= (uint32_t)(Size << 16); 			//Number of bytes
	I2C1->CR2 |= (1U << 25); 						//AUTOEND
	I2C1->CR2 |= (1U << 13); 						//Generate start

	while(Size){									//while size loop
		while( !(I2C1->ISR & (1 << 1)));			//is flag busy?
		I2C1->TXDR = (*data++);						//sen data, data++
		Size--;										//size = size-1;
	}
}
void read_I2C(uint8_t devAddr,uint8_t* data,int Size){		//read function for i2c1 (no address)
	//read Data
	I2C1->CR2 =0;
	I2C1->CR2 |= (uint32_t)(devAddr << 1);
	I2C1->CR2 |= (1U << 10); 						//Read mode
	I2C1->CR2 |= (uint32_t)(Size << 16); 			//Number of bytes
	I2C1->CR2 |= (1U << 25); 						//AUTOEND
	I2C1->CR2 |= (1U << 13); 						//Generate start

	while(Size){
		while( !(I2C1->ISR & (1 << 2)));
		(*data++) = (uint8_t)I2C1->RXDR;
		Size--;
	}
}
void write_memory_I2C(uint8_t devAddr,uint16_t memAddr,uint8_t* data, int Size){
	//Send address and register to read
	I2C1->CR2 =0;
	I2C1->CR2 |= (uint32_t)(devAddr << 1);
	I2C1->CR2 |= (uint32_t)(( Size + 2)<< 16); 		//Number of bytes
	I2C1->CR2 |= (1U << 25); 						//AUTOEND
	I2C1->CR2 |= (1U << 13); 						//Generate start

	while(!(I2C1->ISR & (1 << 1)));					//
	I2C1->TXDR = (uint32_t)(memAddr >> 8);			//(uint32_t)

	while(!(I2C1->ISR & (1 << 1)));					//
	I2C1->TXDR = (uint32_t)(memAddr & 0xFF);		//(uint32_t)

	while(Size){									//while size loop
		while( !(I2C1->ISR & (1 << 1)));			//is flag busy?
		I2C1->TXDR = (*data++);						//sen data, data++
		Size--;										//size = size-1;
	}
}
void random_read_I2C(uint8_t devAddr,uint16_t memAddr,uint8_t* data,int Size){
	//Send address and register to read
	I2C1->CR2 =0;
	I2C1->CR2 |= (uint32_t)(devAddr << 1);
	I2C1->CR2 |= (2U << 16); 						//Number of bytes
	I2C1->CR2 |= (1U << 13); 						//Generate start

	while(!(I2C1->ISR & (1 << 1)));
	I2C1->TXDR = (uint32_t)(memAddr >> 8);			//(uint32_t)

	while(!(I2C1->ISR & (1 << 1)));
	I2C1->TXDR = (uint32_t)(memAddr & 0xFF);		//(uint32_t)

	while(!(I2C1->ISR & (1 << 6)));					//transmission complete

	//read Data
	I2C1->CR2 =0;
	I2C1->CR2 |= (uint32_t)(devAddr << 1);
	I2C1->CR2 |= (1U << 10); 						//Read mode
	I2C1->CR2 |= (uint32_t)(Size << 16); 			//Number of bytes
	I2C1->CR2 |= (1U << 25); 						//AUTOEND
	I2C1->CR2 |= (1U << 13); 						//Generate start


	while(Size){
		while( !(I2C1->ISR & (1 << 2)));
		(*data++) = (uint8_t)I2C1->RXDR;
		Size--;
	}
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
void capture_key(int keycapture){	//capture (switched) key

	if(keycapture != 'Q'){								//Enter Key
		//Exit the start menu
		start =0;
	}
	if(keycapture == 'D'){								//Enter Key
		//Reset the timeout value if  key pressed
		second=0;
		clock_counter=0;
		state++;
	}
	else if(keycapture == '9'){							// +menu key
		//Reset the timeout value if  key pressed
		second=0;
		clock_counter=0;
		state--;
	}
	else if(keycapture == 'C'){							// -menu key
		//Reset the timeout value if  key pressed
		second=0;
		clock_counter=0;
		state++;
	}
	else if(keycapture == 'E'){							//Record Key
		//Reset the timeout value if  key pressed
		second=0;
		clock_counter=0;
		state=3;
	}
	else if(keycapture == '0'){							//Playback Key
		//Reset the timeout value if  key pressed
		second=0;
		clock_counter=0;
		state=4;
	}
	else if(keycapture == 'F'){							//Other Key
		//Reset the timeout value if  key pressed
		second=0;
		clock_counter=0;
		state=5;
	}
	/* @Name:	capture_key
	 * @Brief:	The mechanism abowe is basicly makes the shitfing (the digits) .
	 * There is no point of commenting every line, it does for example;
	 * 					for	digit1=3 and digit0=2 if i press 3
	 * 					digits become; d0 = 3, d1=2, d2=3
	 */
}
void start_state(){					//start state for calculator (stand by)
	lcd_setCursor(1,1);
	lcd_send_string("   Safa BULAT   ");
	lcd_setCursor(2,1);
	lcd_send_string("    141024051   ");
	 /* @Name:	start_state
	 *
	 * @Brief:	this function is stand-by phase of the calculator.
	 * It shows desired 4 digit on the LCD at start. You can activate calculator by pressing F button.
	 * The calculator will return to this stage if no button is pressed for 10 seconds.
	 */
}
void idle_state(){
	char selected_menu[8] = "record";

	lcd_setCursor(1,1);
	lcd_send_string("   IDLE State   ");
	lcd_setCursor(2,1);
	lcd_send_string("Menu?: ");
	lcd_setCursor(2,8);
	lcd_send_string(selected_menu);
	//show IdLE and wait for menu select key

	 /* @Name:	idle_state
	 *
	 * @Brief:	this function is stand-by phase of the calculator.
	 * It shows desired 4 digit on the SSD at start. You can activate calculator by pressing F button.
	 * The calculator will return to this stage if no button is pressed for 10 seconds.
	 */
}
void full_state(){
	lcd_setCursor(1,1);
	lcd_send_string("   FULL State   ");
	lcd_setCursor(2,1);
	lcd_send_string("EEPROMs are FULL");

	 /* @Name:	full_state
	 *
	 * @Brief:	this function is stand-by phase of the calculator.
	 * It shows desired 4 digit on the SSD at start. You can activate calculator by pressing F button.
	 * The calculator will return to this stage if no button is pressed for 10 seconds.
	 */
}
void record_state(){
	int track_num= 3;
	lcd_setCursor(1,1);
	lcd_send_string("  RECORD State  ");
	lcd_setCursor(2,1);
	lcd_send_string("Track?: ");
	lcd_setCursor(2,9);
	lcd_PrintInt(track_num);
//
//	if(pressedKey == '1'){ // if 1 pressed record to 1
//		record[0]= 0x0;//1
//	}
//	else if(pressedKey == '2'){ // if 2 pressed record to 2
//		record[0]= 0x0;//2
//	}
//	else if(pressedKey == '3'){ // if 3 pressed record to 3
//		record[0]= 0x0;//4
//	}
//	else if(pressedKey == '4'){ // if 4 pressed record to 4
//		record[0]= 0x0;//4
//	}

	//show rcd and wait for track select or record

	 /* @Name:	record_state
	 *
	 * @Brief:	this function is stand-by phase of the calculator.
	 * It shows desired 4 digit on the SSD at start. You can activate calculator by pressing F button.
	 * The calculator will return to this stage if no button is pressed for 10 seconds.
	 */
}
void playback_state(){
	int track_num= 3;
	lcd_setCursor(1,1);
	lcd_send_string(" PLAYBACK State ");
	lcd_setCursor(2,1);
	lcd_send_string("Track?: ");
	lcd_setCursor(2,9);
	lcd_PrintInt(track_num);

	//show rcd and wait for track select or playback

	 /* @Name:	playback_state
	 *
	 * @Brief:	this function is stand-by phase of the calculator.
	 * It shows desired 4 digit on the SSD at start. You can activate calculator by pressing F button.
	 * The calculator will return to this stage if no button is pressed for 10 seconds.
	 */
}
void status_state(){
	int track_num= 3;
	lcd_setCursor(1,1);
	lcd_send_string("  STATUS State  ");
	lcd_setCursor(2,1);
	lcd_PrintInt(track_num);
	lcd_setCursor(2,3);
	lcd_send_string("Rec Avaliable");

	//show rcd and wait for track select or status

	 /* @Name:	status_state
	 *
	 * @Brief:	this function is stand-by phase of the calculator.
	 * It shows desired 4 digit on the SSD at start. You can activate calculator by pressing F button.
	 * The calculator will return to this stage if no button is pressed for 10 seconds.
	 */
}
void voice_recorder(){					//Voice recorder main function

	if(start == 1){//idle state
		start_state();
	}
	else if(state == 1){//full_state
		idle_state();
	}
	else if(state == 2){//full_state
		full_state();
	}
	else if(state == 3){//record_state
		record_state();
	}
	else if(state == 4){//playback_state
		playback_state();
	}
	else if(state == 5){//status_state
		status_state();
	}
	else if(state > 5){//reset/clear etc. state
		state=2;
	}
	else if(state < 2){//reset/clear etc. state
		state=5;
	}

	 /* @Name:	voice_recorder
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
void TIM1_BRK_UP_TRG_COM_IRQHandler(void){		//timer1 interrupt handler
	pressedKey= keypad();		//get pressed key
	capture_key(pressedKey);	//send pressed key
	clock_counter++;

	if(clock_counter == 5){		//for 10sec timeout
		clock_counter=0;
		second++;

		GPIOC->ODR ^= (1U << 6);


		if(second==10){			//if 10sec has passed;
			second=0;			//reset second counter
			state=1;			//go to the idle state
//			idle=1;				//go to the idle state
		}
	}
	TIM1->SR	&= ~(1U <<0); 			//clear update status
}
void TIM2_IRQHandler(void){						//timer2 interrupt handler

//	if(regAddr1 < regAddrEnd){
//		random_read_I2C(eeprom_1_address, regAddr1, (uint8_t *)&track, 1);
//		regAddr1++;
//
//		//pwm ile dac  -> speaker'a yolla..
//	}
//	else if(regAddr2 < regAddrEnd){
//		random_read_I2C(eeprom_2_address, regAddr2, (uint8_t *)&track, 1);
//		regAddr2++;
//
//		//pwm ile dac  -> speaker'a yolla..
//	}
//	else{
//		regAddr1=0x0;
//		regAddr2=0x0;
//		//print kayitlar bitti etc..
//	}
//	while(!(I2C1->ISR & (1 << 1)));					//
//	I2C1->TXDR = (uint32_t)(memAddr >> 8);			//(uint32_t)
//
//	while(!(I2C1->ISR & (1 << 1)));					//
//	I2C1->TXDR = (uint32_t)(memAddr & 0xFF);		//(uint32_t)

	mecond++;

	TIM2->CCR2	 = (pwmvalue-180)/2;		//for %50 duty cycle
//	TIM2->CR1 = pwmvalue;				//load pwm value to pwm.

//	TIM2->SR	&= ~(1U <<2); 			//clear update status
	TIM2->SR	&= ~(1U <<0); 			//clear update status
}
void TIM3_IRQHandler(void){						//timer3 interrupt handler
	TIM3->SR	&= ~(1U <<0); 			//clear update status
	//closed
}
void I2C1_IRQHandler(void){						//i2c handler
//	int error=1;
	//only come here if error occurs..
}
void ADC_COMP_IRQHandler(void){					//adc1 interrupt handler
	inputCaptureVal=(uint32_t)(ADC1->DR);
	pwmvalue=inputCaptureVal;

	ADC1->ISR &= (1U <<2); //clear interrupt
	TIM2->ARR	 = pwmvalue;			//for 25600 Hz
}
int main(void) {								//main
	//Enable clock for PORT A and PORT B
	RCC->IOPENR |= 0x3;
	RCC->IOPENR |= (1U << 2); //port c for test
	//**** Function inits ****//
	init_timer1();
	init_timer2();
	init_timer3();

	init_i2c();
	init_adc();

	lcd_init();
	init_keypad();
	//**** conf. PC6 as test led ****//
    GPIOC->MODER &= ~(3U << 2*6);
    GPIOC->MODER |= (1U << 2*6);
    delay(1000000);
    GPIOC->BRR |= (1U << 6);
    //**delete after test**//

    //**startup**//
    lcd_clear_all();						//clear lcd
    delay(100000);							//wait ~10ms
	lcd_setCursor(1,1);						//set cursor: row1
	lcd_send_string("        x       ");	//write x to the middle with no reason
	lcd_setCursor(2,1);						//set cursor: row2
	lcd_send_string("        y       ");	//write y to the middle with no reason
    delay(1000000);							//delay 1sec
    start= 1;								//go to the start state
    lcd_clear_all();
    //**end startup**//
    while(1) {
    	lcd_setCursor(1, 1);
    	lcd_send_string("ADC data: ");
    	lcd_setCursor(2, 1);
    	lcd_PrintInt((int)second);
    	lcd_setCursor(2, 8);
    	lcd_PrintInt((int)inputCaptureVal);
    	lcd_setCursor(2, 14);
    	lcd_PrintInt(mecond);
//    	voice_recorder();					//activate voice recorder

//        write_memory_I2C(eeprom_1_address, 0x3, (uint8_t* )0xAA,1);
//        delay(1000000);
//        a[0]= 0x21;
//        delay(1000000);
//        random_read_I2C(eeprom_1_address, 0x3, (uint8_t* )a, 1);
//
//        write_memory_I2C(eeprom_2_address, 0x0, (uint8_t* )0x35,1);
//        delay(1000000);
//        a[0]= 0x21;
//        delay(1000000);
//        random_read_I2C(eeprom_2_address, 0x0, (uint8_t* )&a[2], 1);
//
////******************************************************************************//
//        lcd_clear_all();
//		lcd_setCursor(1,1);
//		lcd_send_string("1602LCD Display");
//		lcd_setCursor(2,1);
//		lcd_send_string("FUK 7SD i=");
//
//		lcd_setCursor(2,12);
//		lcd_PrintInt(mecond);
//		mecond++;
		if(mecond>=100){ mecond=0; }
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
 * ............................................................................
 * Test LED(s);................................................................
 * (Green LED);			PB6
 * ............................................................................
 */
