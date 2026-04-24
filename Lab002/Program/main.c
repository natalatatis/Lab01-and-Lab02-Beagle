#include "os.h"
#include "stdio.h"
void main() {

    OS_INIT();  

    static int num1, num2, sum;
    static float float1, float2, sum2;

    PRINT("Program: Add Two Numbers\n");

    while (1) {


        PRINT("Enter first number: ");
        READ("%d", &num1);

        PRINT("Enter second number: ");
        READ("%d", &num2);

        sum = num1 + num2;
        PRINT("%d + %d = %d \n", num1, num2, sum);

        PRINT("Enter first float number: ");
        READ("%f", &float1);

        PRINT("Enter second float number: ");
        READ("%f", &float2);

        sum2 = float1 + float2;

        PRINT("%f + %f = %f \n", float1, float2, sum2);
    }
}