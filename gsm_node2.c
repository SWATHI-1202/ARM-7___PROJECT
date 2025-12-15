#include <LPC21xx.H>
#define SOIL 1<<12
#define LED 1<<13
//#include"lcd.h"

void UART0_CONFIGURE(void);
void UART0_TX(unsigned char );
unsigned char UART0_RX(void);
void UART0_STRING(unsigned char* );
void delay_milliseconds(unsigned int );


int main()
{
int flag1=0,flag2=0;
PINSEL0|=0;
IODIR0|=LED;
UART0_CONFIGURE();
init();   //LCD

UART0_STRING("AT\r\n");
delay_milliseconds(2000);
UART0_STRING("AT+CMGF=1\r\n");
delay_milliseconds(2000);



while(1)
{
//if((IOPIN0&SOIL)==0)
if(((IOPIN0>>12)&1)==1)
{
//command(0X80);  
//string("Soil is Dry");
//command(0XC0);
//string("MOTOR : ");
//command(0XC8);
//delay_milliseconds(500);
IOCLR0=LED;
//command(0xC8);
//string(" ON ");
if(flag1==0)
{
UART0_STRING("AT+CMGS=\"+916382966419\"\r\n");
delay_milliseconds(2000);
UART0_STRING("MOTOR IS ON");
//delay_milliseconds(1000);
UART0_TX(0X1A);
delay_milliseconds(2000);
flag1=1;
flag2=0;
}
}
//else if((IOPIN0&SOIL)==1)
else if(((IOPIN0>>12)&1)==0)
{
//command(0X80);
//string("Soil is Wet");
//command(0XC0);
//string("MOTOR : ");
IOSET0=LED;
//command(0XC8);
//string(" OFF");
if(flag2==0)
{
UART0_STRING("AT+CMGS=\"+916382966419\"\r\n");
delay_milliseconds(2000);
UART0_STRING("MOTOR IS OFF");
//delay_milliseconds(1000);
UART0_TX(0X1A);
delay_milliseconds(2000);
flag2=1;
flag1=0;
}
}
}
}


void UART0_CONFIGURE()
{
PINSEL0|=0X5;
U0LCR=0X83;
U0DLL=97;
U0DLM=0;
U0LCR=0X3;
}


void UART0_TX(unsigned char data)
{
while(((U0LSR>>5)&1)==0);
U0THR=data;
}


unsigned char UART0_RX(void)
{
while(((U0LSR>>0)&1)==0);
return U0RBR;
}  


void UART0_STRING(unsigned char *s)
{
while(*s)
{
UART0_TX(*s++);

}
}

 void delay_milliseconds(unsigned int seconds)
{
T0PR=60000-1;
T0TCR=0X01;
while(T0TC<seconds);
T0TCR=0X03;
T0TCR=0X00;
}