#include "serialPort.h"
int main(void)
{
    SerialPort serial;
    if(serial.Open()) {return 1;}

    return 0;
}
