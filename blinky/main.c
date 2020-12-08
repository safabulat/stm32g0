/*
 * main.c
 *
 * author: Furkan Cayci
 *
 * description: Blinks 1 on-board LED at roughly 1 second intervals. system 
 *   clock is running from HSI which is 16 Mhz. Delay function is just a simple 
 *   counter so is not accurate and the delay time will change based on the 
 *   optimization flags.
 */

#include "stm32g0xx.h"

#define LEDDELAY    400000

int main(void);
void delay(volatile uint32_t);


void para1(int a){
	a++;
}
void para2(int a, int b){
	a=a+b;
}
void para3(char a, int b, double c){
	char d[3];
	d[0]=(char )a;
	d[1]=(char )b;
	d[2]=(char )c;
}
void para4(int a, int b, int c, int d){
	a=a+b;
	c=a+c;
	d=c+d;
}
void para5(int a, int b, int c, int d, int e){
	int q[5];
	q[0]=a;
	q[2]=b;
	q[1]=c;
	q[4]=d;
	q[3]=e;
}
void para6(int a, int b, int c, int d, int e,int x){
	int z[6];
	z[0]=a;
	z[2]=b;
	z[1]=c;
	z[4]=d;
	z[3]=e;
	z[5]=x*(a+b+c+d+e);
}



int main(void) {

    /* Enable GPIOC clock */
    RCC->IOPENR |= (1U << 2);

    /* Setup PC6 as output */
    GPIOC->MODER &= ~(3U << 2*6);
    GPIOC->MODER |= (1U << 2*6);

    /* Turn on LED */
    GPIOC->ODR |= (1U << 6);

    para1(3);
    para2(3,2);
    para3('s',7,3.1);
    para4(1,2,3,4);
    para5(9,8,7,6,5);
    para6(1,3,2,4,3,5);



    while(1) {
        delay(LEDDELAY);
        /* Toggle LED */
        GPIOC->ODR ^= (1U << 6);
    }

    return 0;
}

void delay(volatile uint32_t s) {
    for(; s>0; s--);
}
