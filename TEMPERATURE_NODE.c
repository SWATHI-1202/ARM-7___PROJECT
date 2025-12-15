	#include <LPC21xx.h>
	
	void Init_SPI0(void);
	unsigned char SPI0(unsigned char );
	float Read_ADC_MCP3204(unsigned char );
	void can2_init(void);
  void delay_milliseconds(unsigned int );
typedef unsigned int u32;
typedef struct CAN2
{
	u32 id;
	u32 rtr;
	u32 dlc;
	u32 byteA;
	u32 byteB;
}CAN2_MSG;
void can2_tx(CAN2_MSG m1);
// 	void fl(float );
// 	void lcd_str(unsigned char*);
//   void delay_milliseconds(unsigned int);
// 	void integer(int );
// 	void LCD_INIT(void);
// 	void LCD_COMMAND(unsigned char);
// 	
// 	#define LCD_DATA 0XF<<20
// 	#define RS 1<<17
// 	#define RW 1<<18
// 	#define E 1<<19
	
	
	//----Chip Select pin------//
	
	#define CS 7
	
	
	// Initialize SPI0 in Master Mode, Mode 3
	
	void Init_SPI0(void)
	
	{
	
	    PINSEL0 |= 0x00001500;    // Select SPI0 pins: SCK, MOSI, MISO
	
	    S0SPCCR = 150;            // SPI clock ~100 kHz
	
	    S0SPCR  = (1<<5) | 0x18;  // Master mode, Mode 3 (CPOL=1, CPHA=1)
	
	    IODIR0 |= (1 << CS);      // Set CS as output
	
	    IOSET0  = (1 << CS);      // CS high (inactive)
	
	}
	
	
	// Send and receive one byte via SPI0 (bit-shift style check)
	
	unsigned char SPI0(unsigned char data)
	
	{
	
	    S0SPSR;                                 // Clear SPIF
	
	    S0SPDR = data;                          // Load SPI data register
	
	    while (((S0SPSR >> 7) & 1) == 0);      // Wait for transfer complete (SPIF = 1)
	
	    return S0SPDR;                          // Return received byte
	
	}
	
	


	//-------------Read MCP3204 ADC channel (0-3)-----------------//

	
	float Read_ADC_MCP3204(unsigned char channel)
	
	{
	
	    unsigned char highByte, lowByte;
	
	    unsigned int adcValue;
	
	
	    IOCLR0 = (1 << CS);                // CS low: activate ADC
	
	
	    SPI0(0x06);                        // Start bit + single-ended
	
	    highByte = SPI0(channel << 6);     // Send channel, read upper 4 bits
	
	    lowByte  = SPI0(0x00);             // Read lower 8 bits
	
	
	    IOSET0 = (1 << CS);                // CS high: deactivate ADC
	
	
	    adcValue = ((highByte & 0x0F) << 8) | lowByte;  // Combine 12-bit ADC
	
	    return (adcValue * 3.3) / 40.96;               // Convert to voltage
	
	}
	
	
	

	//---------------Main program--------------//
	
	int main()
	
	{
		int temp;
	    CAN2_MSG m1;
		  VPBDIV = 1; 
	
	    can2_init();
	    m1.id=1;
	    m1.rtr=0;//data frame
	    m1.dlc=4;
	    
	
	
	    Init_SPI0();          // Initialize SPI
	
// 	    LCD_INIT();           // Initialize LCD
// 	
// 	    lcd_str("MCP3204--"); // Display static text
	
	    IOSET0 |= (1 << CS);  // Ensure CS is high initially
	
	
	    while(1)
	
	    {
	
	        temp = Read_ADC_MCP3204(0); // Read channel 0
	
// 			voltage=voltage*10;
	
// 	        LCD_COMMAND(0xC0);   // Move cursor to second line
	
// 	        fl(voltage);        // Display voltage
			m1.byteA=	temp;
			m1.byteB=0;
		  can2_tx(m1);
			delay_milliseconds(1000);
	    }
	
	}
	
	
	
        //-------------------LCD setup-------------------------//
	
	
// 	 void LCD_COMMAND(unsigned char CMD){
// 		IOCLR1 = LCD_DATA;
// 		IOSET1 = (CMD&0xf0)<<16;
// 		IOCLR1 = RS;
// 		IOSET1 = E;
// 		delay_milliseconds(2);
// 		IOCLR1 = E;
// 	
// 		IOCLR1 = LCD_DATA;
// 		IOSET1 = (CMD&0x0f)<<20;
// 		IOCLR1 = RS;
// 		IOSET1 = E;
// 		delay_milliseconds(2);
// 		IOCLR1 = E;
// 	}
// 	void LCD_INIT(void)
// 	{
// 		IODIR1 = LCD_DATA|RS|RW|E;
// 		IOCLR1=RW;
// 		LCD_COMMAND(0X01);
// 		LCD_COMMAND(0X02);
// 		LCD_COMMAND(0X0C);
// 		LCD_COMMAND(0X28);
// 	}
// 	
// 	void data(unsigned char DATA)
// 	{
// 		IOCLR1 = LCD_DATA;
// 		IOSET1 =  (DATA&0xf0)<<16;
// 		IOSET1 = RS;
// 		IOSET1 = E;
// 		delay_milliseconds(2);
// 		IOCLR1 = E;
// 	
// 		IOCLR1 = LCD_DATA;
// 		IOSET1 = (DATA&0x0f)<<20;
// 		IOSET1 = RS;
// 		IOSET1 = E;
// 		delay_milliseconds(2);
// 		IOCLR1 = E;
// 	}
// 	void integer(int n)
// 	{
// 		unsigned char arr[5];
// 		signed char i=0;
// 		if(n==0){
// 		data('0');
// 		return;
// 		}
// 		while(n>0)
// 		{
// 		arr[i++]=(n%10);
// 		n=(n/10);}
// 	
// 		for(--i;i>=0;i--)
// 		data(arr[i]+48);
// 	}
// 	
// 	void fl(float a)
// 	{
// 		int temp;
// 		temp=a;
// 		integer(temp);
// 		data('.');
// 		temp=(a-temp)*100;
// 		if(temp<10)data('0');
// 		integer(temp);
// 	}
// 	
// 	void lcd_str(unsigned char * str)
//         {
// 		unsigned char count =0;
// 		while(*str){
// 			data(*str++);
// 			count++;
// 			if(count == 16)
// 				LCD_COMMAND(0XC0);
// 	}
// 	}
	

	//------------------Delay--------------------//

	
 	void delay_milliseconds(unsigned int ms)
		  {    
      T0PR = 60000-1;//Assign the prescale value to PR 
      T0TCR = 0X01;// enable the count registers(T0PC,T0TC)   
      while(T0TC<ms);//wait for the required delay
      T0TCR = 0X03;//Counters reset  
      T0TCR = 0x00;//disable the count registers
    }
		
		void can2_init(void){
	PINSEL1|=0x00014000;//P0.23-->RD2 & P0.24-->TD2
	//VPBDIV=1; //PCLK=60MHz

	C2MOD=0x1; //CAN2 into Reset  Mode 
	AFMR=0x2; //accept all receiving messages(data/remote)
	
	C2BTR=0x001C001D; //B125Kbps @ PLCK=60MHz(BRP=Pclk/bit rate*16)
	
	C2MOD=0x0; //CAN1 into Normal Mode
}

void can2_tx(CAN2_MSG m1)
{
  C2TID1=m1.id;
	C2TFI1=(m1.dlc<<16);	
	if(m1.rtr==0) //if data frame
	{
		C2TFI1&=~(1<<30);//RTR=0
		C2TDA1=m1.byteA; //lower 4bytes of data
		C2TDB1=m1.byteB; //upper 4bytes of data
	}
	else{
		C2TFI1|=(1<<30); //RTR=1
	}
	C2CMR=(1<<0)|(1<<5);//Start Xmission & select Tx Buf1
	while((C2GSR&(1<<3))==0);//wait for data Xmission.
}
	
	
