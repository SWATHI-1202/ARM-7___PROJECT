#include<lpc21xx.h>
#define pin 12
#define led (1<<11)
//void delay_microseconds(unsigned int );

//void delay_ms(unsigned int );
void delay_ms(unsigned int );
void can2_init(void);

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


int main()
{
	CAN2_MSG m1;
	IODIR0=(1<<11);
	can2_init();
	m1.id=2;
	m1.rtr=0;//data frame
	m1.dlc=4;
	while(1)
	{
		if(((IOPIN0>>12)&1)==0)
		{
			IOSET0=led;
      m1.byteA=0;
	    m1.byteB=0;	
	    can2_tx(m1);
      delay_ms(600); 			
		}
		else
		{
			IOCLR0=led;
			m1.byteA=1;
	    m1.byteB=0;	
	    can2_tx(m1);
			delay_ms(600);
		}		
		
	}
	
}


//-------------------C A N------------------------//

void can2_init(void){
	PINSEL1|=0x00014000;//P0.23-->RD2 & P0.24-->TD2
	VPBDIV=1; //PCLK=60MHz

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


// void can2_rx(CAN2_MSG *m1){
// 	while((C2GSR&0x1)==0);
// 	m1->id=C2RID;
// 	m1->dlc=(C2RFS>>16)&0xF;
// 	m1->rtr=(C2RFS>>30)&0x1;
// 	if(m1->rtr==0){ //if data frame
// 		m1->byteA=C2RDA;
// 		m1->byteB=C2RDB;
// 	}
// 	C2CMR=(1<<2);//free receiver buffer(imp)
// }




//--------------------D E L A Y---------------//


void delay_ms(unsigned int seconds)
{
    T0PR = 60000 - 1;   // Adjusted for 60MHz PCLK
    T0TCR = 0x01;
    while (T0TC < seconds);
    T0TCR = 0x03;
    T0TCR = 0x00;
}
