#include <string>
#include <windows.h>
#include <functional>
#include <span>
class SerialPort
{
    const std::string _portName;
    HANDLE _handle;
    public:
       SerialPort(const std::string &portName) : _portName(portName){}
       bool Open();
       void Close();
       bool Write(std::span<const char> buf) const;
       bool Read(std::span<char> str) const;
       virtual ~SerialPort() {Close();};

       void LoopRead(std::function<int(std::span<char>)> callback);
    public:
        static void ScanPorts();
};
