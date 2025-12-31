#include<LPC21XX.H>

#include "lcd_header.h"
#include "i2c.c"
#include "i2c_eeprom.h"
// #include "types.h"
// #include "i2c_defines.h"
 #include "i2c.h"
 #include "i2c_eeprom.c"

#define LED1 1<<17

#define LED2 1<<18

#define IRQ_Slot_En 0x20

#define Eint0_num 14


void config_interrupt(void);


 signed int count=0;

 volatile int inc_flag = 0;

 volatile int dec_flag = 0;


void EXT_INT0_ISR(void) __irq

{

EXTINT = 0X1;

inc_flag = 1;

VICVectAddr =0;

}


void EXT_INT1_ISR(void) __irq

{

EXTINT = 0X2;

dec_flag = 1;

  VICVectAddr =0;

}


int main(){

   
   int prev_count = -1;

 

PINSEL1|= 0X01;//P0.16 is configured as EINT0 pin

   PINSEL0|= 0X20000000; //P.0.14

   PINSEL0 |=0X00000050;
   IODIR0  |= LED1|LED2;
   IOSET0  |= LED1|LED2;
   
   init_i2c();
  
   count = (i2c_eeprom_read(0x50, 0x00) << 8) | i2c_eeprom_read(0x50, 0x01);
  
   config_interrupt();

   lcd_init();

    EXTMODE  = 0X00;

    EXTPOLAR = 0X00;

 VICIntEnable = 1<<Eint0_num|1<<15;

 

 

while(1)

{

if(inc_flag)

        {

            delay_ms(200);  

            count++;

            inc_flag = 0;
            
            i2c_eeprom_write(0x50, 0x00, (count>>8)&0xFF);
            
            i2c_eeprom_write(0x50, 0x01, count&0xFF);
            

        }


       

        if(dec_flag)

        {

            delay_ms(200);  

            if(count > 0)

            count--;

            dec_flag = 0;
            
            i2c_eeprom_write(0x50, 0x00, (count>>8)&0xFF);
            
            i2c_eeprom_write(0x50, 0x01, count&0xFF);

        }
        

        // BUTTON FOR RESET EEPROM COUNT TO 0
        
  if(((IOPIN0>>15)&1)==0) {     // button pressed
      
    delay_ms(200);             // debounce
     
      if(((IOPIN0>>15)&1)==0) {  // still pressed
        
        count = 0;             // reset count
          
        i2c_eeprom_write(0x50,0x00,0x00); // EEPROM MSB
          
        i2c_eeprom_write(0x50,0x01,0x00); // EEPROM LSB
          
        while(((IOPIN0>>15)&1)==0); // wait until button released
      }
  }


if(count>=1)

{

IOCLR0|=LED1;


if(count>=5)

{

IOCLR0|=LED2;

}

else

{

IOSET0|=LED2;

}

}

else

{

count = 0;

IOSET0|=LED1|LED2;


}



if(count != prev_count)

{

   lcd_cmd(0x01);

lcd_cmd(0x80);

lcd_str("COUNT:");

lcd_int(count);

prev_count = count;

}

}


}



void config_interrupt(void)

{

VICIntSelect = 0;//All as IRQs



VICVectCntl0 = IRQ_Slot_En|Eint0_num;

VICVectAddr0 = (int)EXT_INT0_ISR;




VICVectCntl1 = IRQ_Slot_En|15;

VICVectAddr1 = (int)EXT_INT1_ISR;

}
