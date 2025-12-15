  #include<lpc21xx.h>
  #define lcd_d 0XFF<<0
	#define rs 1<<8
	#define E 1<<9
	void Init(void);
	void integer(int);
	void command(unsigned char);
	void data(unsigned char);
	void LCD_STR(unsigned char *);
	void delay_ms(unsigned int );
   
typedef unsigned int u32;
typedef struct CAN2
{
	u32 id;
	u32 rtr;
	u32 dlc;
	u32 byteA;
	u32 byteB;
}CAN2_MSG;

void can2_rx(CAN2_MSG *);
void can2_init(void);

int main()
{
  
	CAN2_MSG m1;
	Init();
	command(0X80);
	LCD_STR("NEAR:");
	delay_ms(1000);
	command(0XC0);
	LCD_STR("LIGHT:");
	delay_ms(1000);
  command(0x94);     // 3rd line
  LCD_STR("TEMP: ");
	can2_init();
	
	
	//uart0_init(115200);
	//uart0_tx_string("nodeB TESTING CAN\r\n");
	while(1){		
		can2_rx(&m1);
		
		if(m1.id==1){
		command(0X80);
	  LCD_STR("NEAR:        ");     
    command(0x86);
    if(m1.byteA > 25)
		{
		integer(m1.byteA);
		command(0x86 + 5);
		LCD_STR("safe");
		}
    else{
		integer(m1.byteA);
		command(0x86 + 3);
		LCD_STR("danger");
		}			
    delay_ms(1000); 			
		}	
		else if(m1.id==2){
		command(0XC0);
	  LCD_STR("LIGHT:       ");
			if(m1.byteA==1){
			command(0XC7);
			LCD_STR("ON"); 				
			}
			else{
			command(0XC7);
			LCD_STR("OFF");
			}
			delay_ms(1000);
		}	
//		else if(m1.id==3)
//		{
//			command(0x94);     
//      LCD_STR("TEMP:        ");
//      command(0x94 + 10);
//      integer(m1.byteA);
//			delay_ms(1000);
//		}
	}
	
}




//---------------------LCD-------------------------//

  void integer(int n)
	{
	unsigned char arr[10];
	signed char i=0;
	while(n>0)
	{
	arr[i++]=(n%10);
	n=(n/10);}

	for(--i;i>=0;i--)
	data(arr[i]+48);
	}
	
	void Init(void)
	{
	IODIR0=lcd_d|rs|E;
	command(0X01);
	command(0X02);
	command(0X0C);
	command(0X38);                                                                                   
	}
	
	void command(unsigned char cmd)
	{
	IOCLR0=lcd_d;
	IOSET0=cmd;
	IOCLR0=rs;
	IOSET0=E;
	delay_ms(2);
	IOCLR0=E;
	}				 
	void data(unsigned char d)
	{
	IOCLR0=lcd_d;
	IOSET0=d;
	IOSET0=rs;
	IOSET0=E;
	delay_ms(2);
	IOCLR0=E;
	}
	
	void LCD_STR(unsigned char *s)
 {
	unsigned char count=0;
	while(*s)
	{
		data(*s++);
		count++;
		if(count==16)
			command(0XC0);
	}
 }
 
 //--------------------D E L A Y---------------//
 
//  void delay_ms(unsigned int seconds)
// {
// T0PR=15000-1;
// T0TCR=0X01;
// while(T0TC<seconds);
// T0TCR=0X03;
// T0TCR=0X00;
// }
void delay_ms(unsigned int seconds)
{
    T0PR = 60000 - 1;   // Adjusted for 60MHz PCLK
    T0TCR = 0x01;
    while (T0TC < seconds);
    T0TCR = 0x03;
    T0TCR = 0x00;
}

	
//------------------C A N----------------------//

void can2_init(void){
	PINSEL1|=0x00014000;//P0.23-->RD2 & P0.24-->TD2
 	VPBDIV = 1;  //PCLK=60MHz

	C2MOD=0x1; //CAN2 into Reset  Mode 
	AFMR=0x2; //accept all receiving messages(data/remote)
	
	C2BTR=0x001C001D; //B125Kbps @ PLCK=60MHz(BRP=Pclk/bit rate*16)
	
	C2MOD=0x0; //CAN1 into Normal Mode
}


void can2_rx(CAN2_MSG *m1){
	while((C2GSR&0x1)==0);
	m1->id=C2RID;
	m1->dlc=(C2RFS>>16)&0xF;
	m1->rtr=(C2RFS>>30)&0x1;
	if(m1->rtr==0){ //if data frame
		m1->byteA=C2RDA;
		m1->byteB=C2RDB;
	}
	C2CMR=(1<<2);//free receiver buffer(imp)
}



