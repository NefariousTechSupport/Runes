/*
	File:
		HardwareInterface.hpp

	Description:
		Base class for communicating with a portal peripheral.
*/

#ifndef RUNES_HARDWARE_INTERFACE_HPP
#define RUNES_HARDWARE_INTERFACE_HPP

#include <atomic>
#include <stdint.h>

#include "PortalType.hpp"

namespace Runes::Portal
{
	enum HardwareErrorCode
	{
		kHWErrNoError,
		kHWErrInvalidPortalType,
		kHWErrUnimplementedPortalType,
		kHWErrNoPortalFound,
		kHWErrAlreadyConnected,
		kHWErrLostConnection,
		kHWErrReadTimedOut,
		kHWErrGenericReadError,
		kHWErrUnknownError,
	};

	class HardwareInterface
	{
	public:
		HardwareInterface(PortalType type);
		virtual ~HardwareInterface();

		bool connected() const;
		inline PortalType getPortalType() const { return _type; }

		virtual void disconnect() = 0;
		virtual HardwareErrorCode writeOut(uint8_t buffer[], size_t len) = 0;
		virtual HardwareErrorCode writeOutEp1(uint8_t buffer[], size_t len) = 0;
		virtual HardwareErrorCode readIn(uint8_t buffer[], size_t len) = 0;

		static constexpr size_t Xbox360BufferHeaderSize   = 2;
		static constexpr size_t EP0WriteSize              = 0x20;
		static constexpr size_t EP0ReadSize               = 0x20;

	protected:
		enum State
		{
			kStateUninitialised,
			kStateConnected,
			kStateErrored
		};

		std::atomic<State>              _state;

	private:
		PortalType                      _type;
	};
}

#endif // RUNES_HARDWARE_INTERFACE_HPP