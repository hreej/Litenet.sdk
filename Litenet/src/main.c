#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
int main()
{
init_platform();
 
xil_printf("Hello World\n\r");
xil_printf("Successfully ran Hello World application");
cleanup_platform();
return 0;
}