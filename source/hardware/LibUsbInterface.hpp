/*
	File:
		LibUsbInterface.hpp

	Description:
		class for interacting with a usb device via libusb.
*/
#ifndef RUNES_LIB_USB_INTERFACE_HPP
#define RUNES_LIB_USB_INTERFACE_HPP

#include "HardwareInterface.hpp"

struct libusb_context;
struct libusb_device;
struct libusb_device_handle;

namespace Runes::Portal
{
	class LibUsbInterface : public HardwareInterface
	{
	public:
		LibUsbInterface();
		virtual ~LibUsbInterface() final;

		HardwareErrorCode connect(libusb_device_handle* handle);
		virtual void disconnect() final;
		virtual HardwareErrorCode writeOut(uint8_t buffer[], size_t len) final;
		virtual HardwareErrorCode writeOutEp1(uint8_t buffer[], size_t len) final;
		virtual HardwareErrorCode readIn(uint8_t buffer[], size_t len) final;

		static LibUsbInterface* poll();

	private:
		static libusb_context*         _libusbCtx;
		libusb_device_handle*          _deviceHandle;
		bool                           _interfaceClaimed;
	};
}

#endif // RUNES_LIB_USB_INTERFACE_HPP
