#include <lpc21xx.h>
void can2_init(void);
void delay_ms(unsigned int);
typedef struct CAN2
{
unsigned int id;
unsigned int rtr;
  unsigned int dlc;
  unsigned int byteA;
  unsigned int byteB;
}CAN2_MSG;

void can2_tx(CAN2_MSG);
void can2_rx(CAN2_MSG *);
typedef unsigned int u32;


int main() {
    CAN2_MSG m1;
    can2_init();   // Initialize CAN controller
    m1.id=1;
    m1.rtr=0;//data frame
    m1.dlc=4;
    IODIR0=1<<17;
    IOSET0=1<<17;
    while(1) {
        int soil = ((IOPIN0 >> 10) & 1);
         
        if(soil == 1)
{
     IOSET0=1<<17;
     m1.byteA=1;
     m1.byteB=0;
     can2_tx(m1);
     delay_ms(200);
}
        else{
            delay_ms(300);
            IOCLR0=1<<17;
            m1.byteA=0;
            m1.byteB=0;
            can2_tx(m1);
            delay_ms(200);
}
        delay_ms(100);
    }
}

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


//void can2_rx(CAN2_MSG *m1){
// while((C2GSR&0x1)==0);
// m1->id=C2RID;
// m1->dlc=(C2RFS>>16)&0xF;
// m1->rtr=(C2RFS>>30)&0x1;
// if(m1->rtr==0){ //if data frame
// m1->byteA=C2RDA;
// m1->byteB=C2RDB;
// }
// C2CMR=(1<<2);//free receiver buffer(imp)
//}

//----------------------d e l a y ----------------//

 void delay_ms(unsigned int seconds)
{
T0PR=60000-1;
T0TCR=0X01;
while(T0TC<seconds);
T0TCR=0X03;
T0TCR=0X00;
}

