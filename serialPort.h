#include <string>
#include <windows.h>
#include <functional>
#include <thread>
#include <vector>
#include <span>
class SerialPort
{
    const int _portNumber;
    HANDLE _handle;
    std::jthread readThread;
    public:
       SerialPort(int portNumber) : _portNumber(portNumber){}
       virtual ~SerialPort(){Close();};

       bool Open();
       void Close();
       bool Write(std::span<const char> buf) const;
       const std::vector<uint8_t> Read() const;

       void LoopRead(std::function<void(std::vector<uint8_t>)>& callback);
    public:
        static void ScanPorts();
};
