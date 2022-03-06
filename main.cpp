#include "serialPort.h"
int main(void)
{
    SerialPort::ScanPorts();
    SerialPort serial("COM1");
    if(!serial.Open()) {return 1;}
    if(!serial.Write("coucou")) {return 2;}
    return 0;
}
