#include "serialPort.h"

#include <array>
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
	const TCHAR *pcCommPort = _portName.c_str();

	//  Open a handle to the specified com port.
	_handle = CreateFile(pcCommPort,
					  GENERIC_READ | GENERIC_WRITE,
					  0,			 //  must be opened with exclusive-access
					  NULL,			 //  default security attributes
					  OPEN_EXISTING, //  must use OPEN_EXISTING
					  0,			 //  not overlapped I/O
					  NULL);		 //  hTemplate must be NULL for comm devices
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

	return true;
}

bool SerialPort::Read(std::span<char> str) const
{
    std::array<uint8_t, 512> buffer;
    DWORD nRead;
    const bool status = ReadFile(
            _handle,
            buffer.data(),
            static_cast<DWORD>(buffer.size()),
            &nRead,
            NULL);

	if(!status)
	{
		printf("SetCommState failed with error %d.\n", GetLastError());
	}
	return status;
}
void SerialPort::LoopRead(std::function<int(std::span<char>)> callback)
{

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

        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
        }
    }

}
