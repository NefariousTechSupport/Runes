/*
	File:
		HardwareInterface.hpp

	Description:
		Base class for communicating with a portal peripheral.
*/

#ifndef RUNES_HARDWARE_INTERFACE_HPP
#define RUNES_HARDWARE_INTERFACE_HPP

#include <stdint.h>

#include "hardware/PortalType.hpp"

namespace Runes::Portal
{
	enum HardwareErrorCode
	{
		kHWErrNoError,	
		kHWErrInvalidPortalType,	
		kHWErrUnimplementedPortalType,	
		kHWErrNoPortalFound,	
		kHWErrUnknownError,	
	};

	class HardwareInterface
	{
	public:
		HardwareInterface();
		virtual ~HardwareInterface();

		virtual HardwareErrorCode connect(PortalType type) = 0;
		virtual int32_t writeOut(uint8_t buffer[], size_t len) = 0;
		virtual int32_t writeOutEp1(uint8_t buffer[], size_t len) = 0;
		virtual int32_t readIn(uint8_t buffer[], size_t len) = 0;

	protected:
		enum State
		{
			kStateUninitialised,
			kStateConnected,
			kStateErrored
		};
		static constexpr size_t EP0WriteSize = 0x20;

		State _state;
	};
}

#endif // RUNES_HARDWARE_INTERFACE_HPP