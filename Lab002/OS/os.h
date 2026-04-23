#ifndef OS_H
#define OS_H

void OS_WRITE(const char *s);
void OS_READ(char *buffer, int max_len);
void OS_INIT(void);
void OS_PUTCHAR(char c);

#endif