#include <string>
#include <windows.h>

class SerialPort
{
    const std::string _portName;
    HANDLE _handle;
    public:
       SerialPort(const std::string &portName) : _portName(portName){}
       bool Open();
       void Close();
       bool Write(const std::string& stringToWrite) const;
       bool Read(std::string& str) const;
       virtual ~SerialPort() {Close();};

    public:
        static void ScanPorts();
};
