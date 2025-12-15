#include <lpc21xx.h>
#include <string.h>

// --- Delay Function ---
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 10000; j++);
}

// --- UART0 Functions for GSM ---
void uart0_init(void) {
    PINSEL0 = 0x00000005;   // Enable TxD0, RxD0
    U0LCR = 0x83;           // 8-bit, no parity, 1 stop bit, DLAB=1
    U0DLL = 97;             // 9600 baud @ 15MHz PCLK
    U0LCR = 0x03;           // DLAB=0
}

void uart0_tx(unsigned char ch) {
    while (!(U0LSR & 0x20));
    U0THR = ch;
}

void uart0_string(char *s) {
    while(*s)
        uart0_tx(*s++);
}

// --- Send SMS Function ---
void send_sms(char *msg) {
    uart0_string("AT+CMGF=1\r\n");       // Text mode
    delay_ms(3000);

    uart0_string("AT+CMGS=\"+918838222069\"\r\n"); // Replace with your number
    delay_ms(3000);

    uart0_string(msg);       // Message text
    uart0_tx(26);            // Ctrl+Z to send
    delay_ms(5000);
}

// --- Main Program ---
int main(void) {
    uart0_init();

    IODIR0 &= ~(1 << 10);    // P0.10 as input (Soil Sensor DO)

    delay_ms(5000);          // Wait for GSM to start

    while(1) {
        int soil = (IOPIN0 >> 10) & 1;   // Read soil sensor

        if (soil == 1) { // Soil is DRY
            send_sms("Soil is DRY. Please water the plants.\r\n");
            delay_ms(10000);
        }
        else { // Soil is WET
            send_sms("Soil is WET. No need to water.\r\n");
            delay_ms(10000);
        }

        delay_ms(20000); // Wait before next reading
    }
}

