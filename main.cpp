#include "serialPort.h"
int main(void)
{
    SerialPort serial("COM1");
    if(serial.Open()) {return 1;}

    return 0;
}
