/*
	File:
		HidUsbInterface.hpp

	Description:
		class for interacting with a windows hid usb device. We use this over
		hidapi since hidapi lacks control transfers for some dumb reason.
*/
#ifndef RUNES_HID_USB_INTERFACE_HPP
#define RUNES_HID_USB_INTERFACE_HPP

#include <hidapi.h>

#include "HardwareInterface.hpp"

namespace Runes::Portal
{
	class HidUsbInterface : public HardwareInterface
	{
	public:
		HidUsbInterface();
		virtual ~HidUsbInterface() final;

		virtual HardwareErrorCode connect(PortalType type) final;
		virtual void disconnect() final;
		virtual HardwareErrorCode writeOut(uint8_t buffer[], size_t len) final;
		virtual HardwareErrorCode writeOutEp1(uint8_t buffer[], size_t len) final;
		virtual HardwareErrorCode readIn(uint8_t buffer[], size_t len) final;

	private:
		hid_device*                    _deviceHandle;
		void*                          _platformHandle;
	};
}

#endif // RUNES_HID_USB_INTERFACE_HPP