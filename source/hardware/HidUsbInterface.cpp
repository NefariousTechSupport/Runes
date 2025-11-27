/*
	File:
		HidUsbInterface.cpp

	Description:
		Implementation of windows hid usb devices.
*/

#include "HidUsbInterface.hpp"

#include "PortalType.hpp"
#include "HardwareIds.hpp"

#include "RunesDebug.hpp"

#include <algorithm>

#if _WIN32
#include <windows.h>
#include <hidsdi.h>
#endif // _WIN32

using namespace Runes::Portal;

#undef min



#if _WIN32
#define _windowsHandle (*reinterpret_cast<HANDLE*>(_platformHandle))
#endif // _WIN32





//=============================================================================
// Constructor for HidUsbInterface
//=============================================================================
HidUsbInterface::HidUsbInterface()
: HardwareInterface()
, _deviceHandle(nullptr)
, _platformHandle(nullptr)
{
}



//=============================================================================
// Destructor for HidUsbInterface
//=============================================================================
HidUsbInterface::~HidUsbInterface()
{
	disconnect();
}



//=============================================================================
// connect: Attempts to connect to a portal, returning 0 on success.
//=============================================================================
HardwareErrorCode HidUsbInterface::connect(PortalType type)
{
	uint16_t pid;
	uint16_t vid;

	// Figure out which Pid and Vid to use

	switch(type)
	{
		case PORTAL_TYPE_DEFAULT:
			pid = kDefaultPortalPID;
			vid = kDefaultPortalVID;
			break;

		case PORTAL_TYPE_XBOX360:
			pid = kXbox360PortalPID;
			vid = kXbox360PortalVID;
			return kHWErrUnimplementedPortalType;

		case PORTAL_TYPE_XBOXONE:
			pid = kXboxOnePortalPID;
			vid = kXboxOnePortalVID;
			return kHWErrUnimplementedPortalType;

		default:
			return kHWErrInvalidPortalType;
	}

	hid_device_info* devInfo = hid_enumerate(vid, pid);

#if _WIN32
	_platformHandle = malloc(sizeof(HANDLE*));
	_windowsHandle = CreateFileA(
		devInfo->path,
		FILE_READ_DATA  | FILE_WRITE_DATA,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS   | CREATE_NEW,
		FILE_FLAG_OVERLAPPED,
		NULL
	);
#endif // _WIN32

	_deviceHandle = hid_open_path(devInfo->path);

	hid_free_enumeration(devInfo);

	if (_deviceHandle)
	{
		_state = kStateConnected;
	}
	else
	{
		const wchar_t* errorMessage = hid_error(NULL);
		RUNES_LOG_WARN("hid_open failed with: {}", errorMessage);
	}

	return _deviceHandle == nullptr ? kHWErrNoPortalFound : kHWErrNoError;
}



//=============================================================================
// disconnect: Disconnect from the device
//=============================================================================
void HidUsbInterface::disconnect()
{
	if (_deviceHandle != nullptr)
	{
		RUNES_LOG_INFO("Lost connection! cleaning up...");

		_state = kStateUninitialised;

		hid_close(_deviceHandle);
		_deviceHandle = nullptr;
	}
	if (_platformHandle != nullptr)
	{
#if _WIN32
		CloseHandle(_windowsHandle);
#endif // _WIN32
		free(_platformHandle);
		_platformHandle = nullptr;
	}
}



//=============================================================================
// writeOut: Write data to the connected device
//=============================================================================
HardwareErrorCode HidUsbInterface::writeOut(uint8_t buffer[], size_t len)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != nullptr, "No device handle exists");
	RUNES_ASSERT(len <= EP0WriteSize, "Write buffer is too large!!");

	uint8_t writeBuffer[EP0WriteSize+1] = {};
	writeBuffer[0] = 0;
	memcpy(&writeBuffer[1], buffer, std::min(EP0WriteSize, len));

	bool success;
#if _WIN32 // Control transfer rather than interrupt
	success = HidD_SetOutputReport(_windowsHandle, writeBuffer, sizeof(writeBuffer));
	if (!success)
	{
		RUNES_LOG_FATAL("HidD_SetOutputReport failed with: {}", GetLastError());
	}
#endif // _WIN32

	return success ? kHWErrNoError : kHWErrLostConnection;
}



//=============================================================================
// writeOutEp1: Write data to endpoint 1 of the connected device
//=============================================================================
HardwareErrorCode HidUsbInterface::writeOutEp1(uint8_t /*buffer*/[], size_t /*len*/)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != nullptr, "No device handle exists");

	// Unimplemented
	RUNES_CRASH();
	return kHWErrNoError;
}



//=============================================================================
// readIn: Read data from the connected device
//=============================================================================
HardwareErrorCode HidUsbInterface::readIn(uint8_t buffer[], size_t len)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != nullptr, "No device handle exists");
	RUNES_ASSERT(len <= EP0ReadSize, "Invalid read size!!");

	HardwareErrorCode error = kHWErrNoError;

	uint8_t readBuffer[EP0ReadSize];

	int bytesRead = hid_read_timeout(_deviceHandle, readBuffer, std::min(sizeof(readBuffer), len), 20);

	if (bytesRead >= 0)
	{
		memcpy(buffer, readBuffer, len);
	}
	else
	{
		error = kHWErrGenericReadError;
		RUNES_LOG_FATAL("hid_read failed with: {}", hid_error(_deviceHandle));
	}

	return error;
}