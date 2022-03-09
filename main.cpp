#include "serialPort.h"
#include <iostream>
int main(void)
{
    std::cout << "Scaning COM ports\n";
    SerialPort::ScanPorts();
    std::cout << "port number to connect to:\n";
    int portNumber;
    
    std::cin >> portNumber;
    SerialPort serial(portNumber);
    if(!serial.Open()) {return 1;}
    if(!serial.Write("coucou")) {return 2;}
    std::function<void(std::vector<uint8_t>)> readCallBack = [](std::vector<uint8_t> vector){std::cout << vector.size() << std::endl;};
    serial.LoopRead(readCallBack);
    while(true){};
    return 0;
}
