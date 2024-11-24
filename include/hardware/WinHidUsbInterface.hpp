/*
	File:
		WinHidUsbInterface.hpp

	Description:
		class for interacting with a windows hid usb device. We use this over
		hidapi since hidapi lacks control transfers for some dumb reason.
*/

#ifndef _WIN32
#error "WinHidUsbInterface.hpp is only to be included on windows platforms"
#endif

#ifndef RUNES_HID_USB_INTERFACE_HPP
#define RUNES_HID_USB_INTERFACE_HPP

#include <windows.h>

#include "HardwareInterface.hpp"

namespace Runes::Portal
{
	class WinHidUsbInterface : public HardwareInterface
	{
	public:
		WinHidUsbInterface();
		virtual ~WinHidUsbInterface() final;

		virtual HardwareErrorCode connect(PortalType type) final;
		virtual int32_t writeOut(uint8_t buffer[], size_t len) final;
		virtual int32_t writeOutEp1(uint8_t buffer[], size_t len) final;
		virtual int32_t readIn(uint8_t buffer[], size_t len) final;

	private:
		HANDLE _deviceHandle;
	};
}

#endif // RUNES_HID_USB_INTERFACE_HPP