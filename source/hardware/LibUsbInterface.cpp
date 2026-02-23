/*
	File:
		LibUsbInterface.cpp

	Description:
		Implementation of windows hid usb devices.
*/

#include "LibUsbInterface.hpp"

#include "PortalType.hpp"
#include "HardwareIds.hpp"

#include "RunesDebug.hpp"

#include <algorithm>

#include <libusb.h>

using namespace Runes::Portal;


#undef min




libusb_context* LibUsbInterface::_libusbCtx = nullptr;

//=============================================================================
// Constructor for LibUsbInterface
//=============================================================================
LibUsbInterface::LibUsbInterface()
: HardwareInterface()
, _deviceHandle(nullptr)
, _interfaceClaimed(false)
{
}



//=============================================================================
// Destructor for LibUsbInterface
//=============================================================================
LibUsbInterface::~LibUsbInterface()
{
	disconnect();
}



//=============================================================================
// connect: Attempts to connect to a portal, returning 0 on success.
//=============================================================================
HardwareErrorCode LibUsbInterface::connect(libusb_device_handle* handle)
{
#if _WIN32
	int res = 0;
#else
	int res = libusb_attach_kernel_driver(handle, 0);
#endif // _WIN32
	if (res >= 0)
	{
		res = libusb_claim_interface(handle, 0);
		_deviceHandle = handle;

		RUNES_ASSERT(res >= 0, "Failed to claim interface");
		_interfaceClaimed = res >= 0;

		_state.store(kStateConnected);
	}

	return res == 0 ? kHWErrNoError : kHWErrUnknownError;
}



//=============================================================================
// disconnect: Disconnect from the device
//=============================================================================
void LibUsbInterface::disconnect()
{
	if (_deviceHandle != nullptr)
	{
		RUNES_LOG_INFO("Lost connection! cleaning up...");

		_state.store(kStateUninitialised);

		if (_interfaceClaimed)
		{
			libusb_release_interface(_deviceHandle, 0);
			_interfaceClaimed = false;
		}

		libusb_close(_deviceHandle);
		_deviceHandle = nullptr;
	}
}



//=============================================================================
// writeOut: Write data to the connected device
//=============================================================================
HardwareErrorCode LibUsbInterface::writeOut(uint8_t buffer[], size_t len)
{
	RUNES_ASSERT(_state.load() == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != nullptr, "No device handle exists");
	RUNES_ASSERT(len <= EP0WriteSize, "Write buffer is too large!!");

	uint8_t writeBuffer[EP0WriteSize] = {};
	memcpy(writeBuffer, buffer, std::min(EP0WriteSize, len));


	int res = libusb_control_transfer(_deviceHandle, 0x21, 0x09, 0x0200, 0x00, writeBuffer, len, 20);

	return res >= 0 ? kHWErrNoError : kHWErrLostConnection;
}



//=============================================================================
// writeOutEp1: Write data to endpoint 1 of the connected device
//=============================================================================
HardwareErrorCode LibUsbInterface::writeOutEp1(uint8_t /*buffer*/[], size_t /*len*/)
{
	RUNES_ASSERT(_state.load() == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != nullptr, "No device handle exists");

	// Unimplemented
	RUNES_CRASH();
	return kHWErrNoError;
}



//=============================================================================
// readIn: Read data from the connected device
//=============================================================================
HardwareErrorCode LibUsbInterface::readIn(uint8_t buffer[], size_t len)
{
	RUNES_ASSERT(_state.load() == kStateConnected, "Invalid state for writing data out");
	RUNES_ASSERT(_deviceHandle != nullptr, "No device handle exists");
	RUNES_ASSERT(len <= EP0ReadSize, "Invalid read size!!");

	HardwareErrorCode error = kHWErrNoError;

	uint8_t readBuffer[EP0ReadSize];

	int bytesRead = 0;
	int res = libusb_bulk_transfer(_deviceHandle, 0x81, readBuffer, std::min(sizeof(readBuffer), len), &bytesRead, 5000);

	if (res == 0 && bytesRead >= 0)
	{
		memcpy(buffer, readBuffer, len);
	}
	else
	{
		error = kHWErrGenericReadError;
		RUNES_LOG_FATAL("libusb_interrupt_transfer failed with: {}", res);
	}

	return error;
}



//=============================================================================
// poll: poll for devices
//=============================================================================
LibUsbInterface* LibUsbInterface::poll()
{
	if (!_libusbCtx)
	{
		libusb_init(&_libusbCtx);
	}

	libusb_device_handle* handle = libusb_open_device_with_vid_pid(_libusbCtx, kPortalVID, kDefaultPortalPID);

	LibUsbInterface* device = nullptr;
	if (handle)
	{
		device = new LibUsbInterface();
		device->connect(handle);
	}

	return device;
}
