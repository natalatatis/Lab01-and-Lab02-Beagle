extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);

/* UART0 base (BeagleBone Black AM335x) */
#define UART0_BASE     0x44E09000

#define UART_THR       (UART0_BASE + 0x00)
#define UART_RHR       (UART0_BASE + 0x00)
#define UART_LSR       (UART0_BASE + 0x14)
#define UART_LCR       (UART0_BASE + 0x0C)
#define UART_DLL       (UART0_BASE + 0x00)
#define UART_DLH       (UART0_BASE + 0x04)
#define UART_MDR1      (UART0_BASE + 0x20)

#define UART_LSR_THRE  0x20

/* Clock + control registers */
#define CM_WKUP_BASE        0x44E00400
#define CM_WKUP_UART0_CLKCTRL (CM_WKUP_BASE + 0xB4)

/* Enable UART0 clock */
void uart_init(void) {
    /* Enable UART0 module clock */
    PUT32(CM_WKUP_UART0_CLKCTRL, 0x2);

    /* Disable UART before config */
    PUT32(UART_MDR1, 0x7);

    /* Enable access to divisor latches */
    PUT32(UART_LCR, 0x80);

    /* Set baud rate = 115200 (assuming 48MHz clock) */
    PUT32(UART_DLL, 26);
    PUT32(UART_DLH, 0);

    /* 8-bit, no parity, 1 stop bit */
    PUT32(UART_LCR, 0x03);

    /* Enable UART (16x mode) */
    PUT32(UART_MDR1, 0x0);
}

void uart_send(unsigned char x) {
    while ((GET32(UART_LSR) & UART_LSR_THRE) == 0);
    PUT32(UART_THR, x);
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_send('\r'); // Proper terminal formatting
        uart_send(*s++);
    }
}

int main(void) {
    uart_init();
    uart_puts("Hello World\n");

    while (1);
}