#include "stdio.h"
#include "os.h"
#include "string.h"
#include <stdarg.h>

#define INPUT_BUFFER_SIZE 64


static void itoa(int num, char *buffer) {
    int i = 0;
    int sign = 0;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (num < 0) {
        sign = 1;
        num = -num;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (sign)
        buffer[i++] = '-';

    buffer[i] = '\0';

    // reverse string
    for (int j = 0; j < i / 2; j++) {
        char tmp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = tmp;
    }
}

//Float to string
static void ftoa(float num, char *buffer, int decimals){
    int idx = 0;

    if(num < 0){
        buffer[idx++] = '-';
        num = -num;
    }

    int int_part = (int)num;
    float frac = num - (float)int_part;

    char int_buf[16];
    itoa(int_part, int_buf);

    int i = 0;
    while(int_buf[i]){
        buffer[idx++] = int_buf[i++];
    }

    buffer[idx++] = '.';

    for(int i = 0; i < decimals; i++){
        frac *= 10.0f;
        int digit = (int)(frac + 0.5f); // rounding
        buffer[idx++] = '0' + digit;
        frac -= digit;
    }

    buffer[idx] = '\0';
}

static int atoi_simple(const char *s) {
    int num = 0;
    int sign = 1;

    // Skip leading whitespace or junk
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;

    if (*s == '-') {
        sign = -1;
        s++;
    }

    while (*s >= '0' && *s <= '9') {
        num = num * 10 + (*s - '0');
        s++;
    }
    return sign * num;
}

//String to float method
static float atof_simple(const char *s) {
    int int_part = 0;
    float frac_part = 0.0f;
    float divisor = 10.0f;
    int sign = 1;

    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;

    if (*s == '-') {
        sign = -1;
        s++;
    }

    while (*s >= '0' && *s <= '9') {
        int_part = int_part * 10 + (*s - '0');
        s++;
    }

    if (*s == '.') {
        s++;
        while (*s >= '0' && *s <= '9') {
            frac_part += (*s - '0') / divisor; 
            divisor *= 10.0f;
            s++;
        }
    }

    return (float)sign * ((float)int_part + frac_part);
}

//PRINT

void PRINT(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[64]; // Increased size for safety

    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            fmt++;
            if (*fmt == 'd') {
                int val = va_arg(args, int);
                itoa(val, buf);
                OS_WRITE(buf);
            }
            else if(*fmt == 's') {
                char *s = va_arg(args, char *);
                if (s) OS_WRITE(s);
                else OS_WRITE("(null)");
            }
            else if(*fmt == 'f'){
                // Variadic floats are always passed as doubles
                double d = va_arg(args, double);
                ftoa((float)d, buf, 2);
                OS_WRITE(buf);
            }
        } else {
            // Write single character efficiently
            OS_PUTCHAR(*fmt);
        }
        fmt++;
    }
    va_end(args);
}


//READ
void READ(const char *fmt, void *out) {
    char buffer[INPUT_BUFFER_SIZE];

    // 1. Initialize buffer with null terminators
    for (int i = 0; i < INPUT_BUFFER_SIZE; i++) {
        buffer[i] = '\0';
    }

    // 2. Call the OS layer to get the string
    OS_READ(buffer, INPUT_BUFFER_SIZE);  

    // 3. Perform conversion based on format string
    if (fmt[0] == '%' && fmt[1] == 'd') {
        // Convert to integer
        *(int *)out = atoi_simple(buffer);
        
    }
    else if (fmt[0] == '%' && fmt[1] == 's') {
        // Copy string
        my_strncpy((char *)out, buffer, INPUT_BUFFER_SIZE);
    }
    else if (fmt[0] == '%' && fmt[1] == 'f') {
        // Convert to float
        *(float *)out = atof_simple(buffer);
    }
}