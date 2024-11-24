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
{
}



//=============================================================================
// Destructor for WinHidUsbInterface
//=============================================================================
WinHidUsbInterface::~WinHidUsbInterface()
{
	if (_deviceHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_deviceHandle);
	}
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

			if(HidD_GetAttributes(hDevice, &attributes))
			{
				// Does it match the portal?
				if (attributes.ProductID == pid && attributes.VendorID == vid)
				{
					_deviceHandle = hDevice;
					_state = kStateConnected;
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
// writeOut: Write data to the connected device
//=============================================================================
int32_t WinHidUsbInterface::writeOut(uint8_t buffer[], size_t len)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != INVALID_HANDLE_VALUE, "No device handle exists");
	RUNES_ASSERT(len <= EP0WriteSize, "Write buffer is too large!!");

	uint8_t writeBuffer[EP0WriteSize+1];
	writeBuffer[0] = 0;
	memcpy(&writeBuffer[1], buffer, std::min(EP0WriteSize, len));

	bool success = HidD_SetOutputReport(_deviceHandle, writeBuffer, sizeof(writeBuffer));
	RUNES_ASSERT(success, "failed to send control transfer! error code %ld", GetLastError());

	return success ? len : 0;
}



//=============================================================================
// writeOutEp1: Write data to endpoint 1 of the connected device
//=============================================================================
int32_t WinHidUsbInterface::writeOutEp1(uint8_t /*buffer*/[], size_t /*len*/)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != INVALID_HANDLE_VALUE, "No device handle exists");

	// Unimplemented
	RUNES_CRASH();
	return 0;
}



//=============================================================================
// readIn: Read data from the connected device
//=============================================================================
int32_t WinHidUsbInterface::readIn(uint8_t buffer[], size_t len)
{
	RUNES_ASSERT(_state == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != INVALID_HANDLE_VALUE, "No device handle exists");

	DWORD bytesRead = 0;
	ReadFile(_deviceHandle, &buffer, len, &bytesRead, NULL);

	return bytesRead;
}