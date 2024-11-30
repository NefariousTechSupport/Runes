/*
	File:
		WinHidUsbInterface.cpp

	Description:
		Implementation of windows hid usb devices.
*/

#include "hardware/WinHidUsbInterface.hpp"

#include "hardware/PortalType.hpp"
#include "hardware/HardwareIds.hpp"

#include "RunesDebug.hpp"

#include <algorithm>

#include <hidsdi.h>
#include <setupapi.h>

using namespace Runes::Portal;



//=============================================================================
// Constructor for WinHidUsbInterface
//=============================================================================
WinHidUsbInterface::WinHidUsbInterface()
: HardwareInterface()
, _deviceHandle(INVALID_HANDLE_VALUE)
, _overlapped()
{
}



//=============================================================================
// Destructor for WinHidUsbInterface
//=============================================================================
WinHidUsbInterface::~WinHidUsbInterface()
{
	disconnect();
}



//=============================================================================
// connect: Attempts to connect to a portal, returning 0 on success.
//=============================================================================
HardwareErrorCode WinHidUsbInterface::connect(PortalType type)
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

	GUID hidGuid;
	HidD_GetHidGuid(&hidGuid);

	SP_DEVICE_INTERFACE_DATA deviceData;
	deviceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	DWORD index = 0;

	// Start looping over devices

	HANDLE devInfoSet = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE);
	if (devInfoSet == INVALID_HANDLE_VALUE)
	{
		return kHWErrUnknownError;
	}
	while (_state != kStateConnected && SetupDiEnumDeviceInterfaces(devInfoSet, NULL, &hidGuid, index, &deviceData))
	{
		DWORD requiredSize;

		SetupDiGetDeviceInterfaceDetail(devInfoSet, &deviceData, NULL, 0, &requiredSize, NULL);

		PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetail = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(requiredSize));
		RUNES_ASSERT(deviceDetail, "failed to allocate device detail struct!!");
		deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		if(SetupDiGetDeviceInterfaceDetail(devInfoSet, &deviceData, deviceDetail, requiredSize, &requiredSize, NULL))
		{
			// Create a readonly file to read the device info

			HANDLE hDevice = CreateFile(deviceDetail->DevicePath,
			                            FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN,
			                            FILE_SHARE_READ | FILE_SHARE_WRITE,
			                            NULL,
			                            CREATE_ALWAYS | CREATE_NEW,
			                            0,
			                            NULL);

			HIDD_ATTRIBUTES attributes;
			attributes.Size = sizeof(HIDD_ATTRIBUTES);

			if(HidD_GetAttributes(hDevice, &attributes))
			{
				// Does it match the portal?
				if (attributes.ProductID == pid && attributes.VendorID == vid)
				{
					_deviceHandle = hDevice;
					_state = kStateConnected;

					_overlapped = {};
					_overlapped.hEvent = CreateEvent(NULL, false, false, NULL);

					RUNES_ASSERT(_overlapped.hEvent != NULL, "Creation of overlapped event returned error %ld", GetLastError());
				}
			}

			if (_state != kStateConnected)
			{
				// If we're connected then hold on to the handle and close it on destruction
				CloseHandle(hDevice);
				hDevice = NULL;
			}
		}

		free(deviceDetail);

		index++;

		SetupDiDeleteDeviceInterfaceData(devInfoSet, &deviceData);
	}
	SetupDiDestroyDeviceInfoList(devInfoSet);

	return _state == kStateConnected ? kHWErrNoError : kHWErrNoPortalFound;
}



//=============================================================================
// disconnect: Disconnect from the device
//=============================================================================
void WinHidUsbInterface::disconnect()
{
	if (_deviceHandle != INVALID_HANDLE_VALUE)
	{
		RUNES_LOG_INFO("Lost connection! cleaning up...");

		_state = kStateUninitialised;

		CloseHandle(_deviceHandle);
		_deviceHandle = INVALID_HANDLE_VALUE;

		CloseHandle(_overlapped.hEvent);
		_overlapped = {};
	}
}



//=============================================================================
// writeOut: Write data to the connected device
//=============================================================================
HardwareErrorCode WinHidUsbInterface::writeOut(uint8_t buffer[], size_t len)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != INVALID_HANDLE_VALUE, "No device handle exists");
	RUNES_ASSERT(len <= EP0WriteSize, "Write buffer is too large!!");

	uint8_t writeBuffer[EP0WriteSize+1];
	writeBuffer[0] = 0;
	memcpy(&writeBuffer[1], buffer, std::min(EP0WriteSize, len));

	bool success = HidD_SetOutputReport(_deviceHandle, writeBuffer, sizeof(writeBuffer));
	if (!success)
	{
		disconnect();
	}
	//RUNES_ASSERT(success, "failed to send control transfer! error code %ld", GetLastError());

	return success ? kHWErrNoError : kHWErrLostConnection;
}



//=============================================================================
// writeOutEp1: Write data to endpoint 1 of the connected device
//=============================================================================
HardwareErrorCode WinHidUsbInterface::writeOutEp1(uint8_t /*buffer*/[], size_t /*len*/)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != INVALID_HANDLE_VALUE, "No device handle exists");

	// Unimplemented
	RUNES_CRASH();
	return kHWErrNoError;
}



//=============================================================================
// readIn: Read data from the connected device
//=============================================================================
HardwareErrorCode WinHidUsbInterface::readIn(uint8_t buffer[], size_t len)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != INVALID_HANDLE_VALUE, "No device handle exists");

	HardwareErrorCode error = kHWErrNoError;

	uint8_t readBuffer[EP0ReadSize+1];

	DWORD bytesRead = 0;
	ResetEvent(_overlapped.hEvent);
	bool success = ReadFile(_deviceHandle, readBuffer, std::min(sizeof(readBuffer), len+1), &bytesRead, &_overlapped);
	if (!success)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			CancelIo(_deviceHandle);
			disconnect();
			error = kHWErrLostConnection;
		}

		int res = WaitForSingleObject(_overlapped.hEvent, 100);
		if (res != WAIT_OBJECT_0)
		{
			RUNES_LOG_WARN("Waiting for read returned %d, error %ld", res, GetLastError());
			error = kHWErrReadTimedOut;
		}

		success = GetOverlappedResult(_deviceHandle, &_overlapped, &bytesRead, false);
	}

	if (success && bytesRead > 0)
	{
		memcpy(buffer, readBuffer, bytesRead);
	}
	else
	{
		error = kHWErrGenericReadError;
	}

	return error;
}