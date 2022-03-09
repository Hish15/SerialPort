#include "serialPort.h"

#include <array>
#include <chrono>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>

void PrintCommState(DCB dcb)
{
    //  Print some of the DCB structure values
    _tprintf( TEXT("\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n"), 
              dcb.BaudRate, 
              dcb.ByteSize, 
              dcb.Parity,
              dcb.StopBits );
}


bool SerialPort::Open()
{
    using namespace std::literals::string_literals;
    std::string portName =  "COM"s + std::to_string(_portNumber);
    const TCHAR *pcCommPort = portName.c_str();

    //  Open a handle to the specified com port.
    _handle = CreateFile(pcCommPort,
                      GENERIC_READ | GENERIC_WRITE,
                      0,             //  must be opened with exclusive-access
                      NULL,             //  default security attributes
                      OPEN_EXISTING, //  must use OPEN_EXISTING
                      0,             //  not overlapped I/O
                      NULL);         //  hTemplate must be NULL for comm devices
    if (_handle == INVALID_HANDLE_VALUE){ return false;}

    if (_handle == INVALID_HANDLE_VALUE)
    {
        //  Handle the error.
        printf("CreateFile failed with error %d.\n", GetLastError());
        return false;
   }

    DCB dcb;
   //  Initialize the DCB structure.
   SecureZeroMemory(&dcb, sizeof(DCB));
   dcb.DCBlength = sizeof(DCB);

   //  Build on the current configuration by first retrieving all current
   //  settings.
   if (!GetCommState(_handle, &dcb)) 
   {
      //  Handle the error.
      printf ("GetCommState failed with error %d.\n", GetLastError());
      return false;
   }

   PrintCommState(dcb);       //  Output to console

   //  Fill in some DCB values and set the com state: 
   //  57,600 bps, 8 data bits, no parity, and 1 stop bit.
   dcb.BaudRate = CBR_57600;     //  baud rate
   dcb.ByteSize = 8;             //  data size, xmit and rcv
   dcb.Parity   = NOPARITY;      //  parity bit
   dcb.StopBits = ONESTOPBIT;    //  stop bit
   if (!SetCommState(_handle, &dcb))
   {
      //  Handle the error.
      printf ("SetCommState failed with error %d.\n", GetLastError());
      return false;
   }

    //Setting Timeouts
    COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
    //The maximum time allowed to elapse before the arrival of the next byte on the communications line
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutConstant = 10;
    if (SetCommTimeouts(_handle, &timeouts) == FALSE)
    {
        printf("Error to Setting Time outs\n");
        return false;
    }

    return true;
}

const std::vector<uint8_t> SerialPort::Read() const
{
    auto buffer = std::vector<uint8_t>();
    buffer.reserve(4096);
    DWORD nRead;
    const bool status = ReadFile(
            _handle,
            buffer.data(),
            static_cast<DWORD>(buffer.capacity()),
            &nRead,
            NULL);
    if(nRead > 0)
    {
        return buffer;
    }
    if(!status)
    {
        printf("SetCommState failed with error %d.\n", GetLastError());
        return std::vector<uint8_t>();
    }
    return buffer;
}
void SerialPort::LoopRead(std::function<void(std::vector<uint8_t>)>& callback)
{
    auto readFunction = [&callback, this]()
    {
        while(true)
        {
            const std::vector<uint8_t> readbytes = Read();
            if(readbytes.size() > 0)
            {
                callback(readbytes); 
            }
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms);
        }
    };

    readThread = std::jthread(readFunction);
}
bool SerialPort::Write(std::span<const char> str) const
{
    DWORD bytesWritten;
    const bool status = WriteFile(
        _handle,
        str.data(),
        static_cast<DWORD>(str.size()),
        &bytesWritten, NULL);
    if(!status)
    {
        printf("SetCommState failed with error %d.\n", GetLastError());
    }
    return status;
}

void SerialPort::Close()
{
    if(_handle != nullptr)
    {
        CloseHandle(_handle);
        _handle = nullptr;
    }
}

void SerialPort::ScanPorts()
{
    char lpTargetPath[5000]; // buffer to store the path of the COMPORTS

    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        const std::string str = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
        const DWORD status = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

        // Test the return value and error if any
        if (status != 0) //QueryDosDevice returns zero if it didn't find an object
        {
            std::cout << str << ": " << lpTargetPath << std::endl;
        }
    }
}
