#include <lpc21xx.h>
#define lcd_d 0xFF << 0
#define rs 1 << 8
#define E 1 << 9

typedef unsigned int u32;

typedef struct CAN2 {
    u32 id;
    u32 rtr;
    u32 dlc;
    unsigned int byteA;
    unsigned int byteB;
} CAN2_MSG;

void Init(void);
void integer(int);
void command(unsigned char);
void data(unsigned char);
void LCD_STR(unsigned char *);
void delay_ms(unsigned int);
void can2_init(void);
void can2_rx(CAN2_MSG *);

int main() {
    CAN2_MSG m1;
    can2_init();
    Init();
    command(0X80);
    LCD_STR("SOIL  STATUS:");
    delay_ms(2000);
    while (1) {
        can2_rx(&m1);
        if (m1.rtr == 0) {
            if (m1.byteA == 1) {
                command(0X01);
                command(0XC0);
                LCD_STR("DRY...");
	  delay_ms(200);

            } else {
                command(0X01);
                command(0XC0);
                LCD_STR("WET...");
	  delay_ms(200);
            }
        }
    }

    return 0;
}

//---------------------LCD-------------------------//
void Init(void) {
    IODIR0 = lcd_d | rs | E;
    command(0X01);
    command(0X02);
    command(0X0C);
    command(0X38);
}

void command(unsigned char cmd) {
    IOCLR0 = lcd_d;
    IOSET0 = cmd;
    IOCLR0 = rs;
    IOSET0 = E;
    delay_ms(2);
    IOCLR0 = E;
}

void data(unsigned char d) {
    IOCLR0 = lcd_d;
    IOSET0 = d;
    IOSET0 = rs;
    IOSET0 = E;
    delay_ms(2);
    IOCLR0 = E;
}

void LCD_STR(unsigned char *s) {
    unsigned char count = 0;
    while (*s) {
        data(*s++);
        count++;
        if (count == 16)
            command(0XC0);
    }
}

void delay_ms(unsigned int seconds) {
    T0PR = 60000 - 1;
    T0TCR = 0X01;
    while (T0TC < seconds);
    T0TCR = 0X03;
    T0TCR = 0X00;
}

//------------------CAN----------------------//
void can2_init(void) {
    PINSEL1 |= 0x00014000; // P0.23-->RD2 & P0.24-->TD2
    VPBDIV = 1;            // PCLK=60MHz
    C2MOD = 0x1;           // CAN2 into Reset Mode
    AFMR = 0x2;            // accept all receiving messages
    C2BTR = 0x001C001D;    // 125Kbps @ PCLK=60MHz
    C2MOD = 0x0;           // CAN2 Normal Mode
}

void can2_rx(CAN2_MSG *m1) {
    while ((C2GSR & 0x1) == 0);
    m1->id = C2RID;
    m1->dlc = (C2RFS >> 16) & 0xF;
    m1->rtr = (C2RFS >> 30) & 0x1;
    if (m1->rtr == 0) {
        m1->byteA = C2RDA;
        m1->byteB = C2RDB;
    }
    C2CMR = (1 << 2); // free receiver buffer
}
