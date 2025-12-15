#include<lpc21xx.h>
unsigned int echo(void);
#define trig (1<<8)
void delay_microseconds(unsigned int );
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
	unsigned int distance;
	unsigned int distance_cm;
	VPBDIV = 1; 
	
	can2_init();
	m1.id=1;
	m1.rtr=0;//data frame
	m1.dlc=4;
	IODIR0=trig;
	while(1)
	{
	IOSET0=trig;
	delay_microseconds(10);
	IOCLR0=trig;
	distance=echo();
	distance_cm=distance/58;
	m1.byteA=distance_cm;
	m1.byteB=0;	
	can2_tx(m1);
	delay_ms(1000);
 }
}


unsigned int echo()
{ 
	unsigned int time;
	while(((IOPIN0>>9)&1)==0);
	T0PR=60-1;
	T0TCR=0X02;
	T0TCR=0X01;
	while(((IOPIN0>>9)&1)==1);
	T0TCR=0X00;
	time=T0TC;
	T0TCR=0X02;
	return time;	
}

//---------------------------DELAY------------------------------//

 void delay_ms(unsigned int ms)
    {    
      T0PR = 60000-1;//Assign the prescale value to PR 
      T0TCR = 0X01;// enable the count registers(T0PC,T0TC)   
      while(T0TC<ms);//wait for the required delay
      T0TCR = 0X03;//Counters reset  
      T0TCR = 0x00;//disable the count registers
    }
		
void delay_microseconds(unsigned int microsec)
{
	T0PR = 60-1;//Assign the prescale value to PR 
  T0TCR = 0X01;// enable the count registers(T0PC,T0TC)
  while(T0TC<microsec);//wait for the required delay
  T0TCR = 0X03;//Counters reset  
	T0TCR = 0x00;//disable the count registers
}


//-------------------C A N------------------------//

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






