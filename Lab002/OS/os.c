#include "os.h"

extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);

/* UART0 base */
#define UART0_BASE     0x44E09000
#define UART_THR       (UART0_BASE + 0x00)
#define UART_RHR       (UART0_BASE + 0x00)
#define UART_DLL       (UART0_BASE + 0x00)
#define UART_DLH       (UART0_BASE + 0x04)
#define UART_FCR       (UART0_BASE + 0x08)
#define UART_LCR       (UART0_BASE + 0x0C)
#define UART_LSR       (UART0_BASE + 0x14)
#define UART_MDR1      (UART0_BASE + 0x20)
#define UART_SYSC      (UART0_BASE + 0x54)
#define UART_SYSS      (UART0_BASE + 0x58)

#define UART_LSR_THRE  0x20
#define UART_LSR_DR    0x01

/* Clock & Control Module */
#define CM_WKUP_BASE          0x44E00400
#define CM_WKUP_UART0_CLKCTRL (CM_WKUP_BASE + 0xB4)
#define CONTROL_MODULE_BASE   0x44E10000
#define CONF_UART0_RXD        (CONTROL_MODULE_BASE + 0x970)
#define CONF_UART0_TXD        (CONTROL_MODULE_BASE + 0x974)

static void uart_init() {
    /* 1. Configure Pinmux for UART0 */
    /* RXD: Mode 0, Pull-up, Input Enabled (0x20 | 0x10 | 0x00) */
    PUT32(CONF_UART0_RXD, 0x30); 
    /* TXD: Mode 0, Pull-up (0x10 | 0x00) */
    PUT32(CONF_UART0_TXD, 0x10);

    /* 2. Module Soft Reset */
    PUT32(UART_SYSC, 0x2);
    while((GET32(UART_SYSS) & 0x1) == 0); 

    /* 3. Enable UART0 module clock */
    PUT32(CM_WKUP_UART0_CLKCTRL, 0x2);

    /* 4. Configuration Sequence */
    PUT32(UART_MDR1, 0x7);   // Disable UART
    PUT32(UART_LCR, 0x83);   // DLAB=1, 8-bit
    PUT32(UART_DLL, 26);     // 115200 Baud
    PUT32(UART_DLH, 0);
    PUT32(UART_LCR, 0x03);   // DLAB=0, 8-N-1
    PUT32(UART_FCR, 0x07);   // Enable/Clear FIFOs
    PUT32(UART_MDR1, 0x0);   // Enable UART (16x Mode)
}

static char uart_getc() {
    while ((GET32(UART_LSR) & UART_LSR_DR) == 0);
    return (char)(GET32(UART_RHR) & 0xFF);
}

static void uart_putc(char c) {
    while ((GET32(UART_LSR) & UART_LSR_THRE) == 0);
    PUT32(UART_THR, c);
}

static void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

static void uart_gets_input(char *buffer, int max_length) {
    int i = 0;
    char c;
    while (i < max_length - 1) {
        c = uart_getc();
        if (c == '\r' || c == '\n') {
            uart_puts("\r\n");
            break;
        }
        if (c == '\b' || c == 127) {
            if (i > 0) {
                i--;
                uart_puts("\b \b");
            }
            continue;
        }
        if (c >= 32 && c <= 126) {
            uart_putc(c);
            buffer[i++] = c;
        }
    }
    buffer[i] = '\0';
}

void OS_WRITE(const char *s) { uart_puts(s); }
void OS_READ(char *buffer, int max_len) { uart_gets_input(buffer, max_len); }
void OS_INIT() { uart_init(); }