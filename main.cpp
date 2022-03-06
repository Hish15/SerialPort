#include "serialPort.h"
int main(void)
{
    SerialPort serial;
    if(serial.Open("COM1")) {return 1;}

    return 0;
}
