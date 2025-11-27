/*
	File:
		PortalDriver.hpp

	Description:
		Instance of a connection to a portal.
*/

#ifndef RUNES_HARDWARE_PORTAL_DRIVER_HPP
#define RUNES_HARDWARE_PORTAL_DRIVER_HPP

#include <cstdint>
#include <thread>
#include <atomic>
#include <array>

#include "HardwareInterface.hpp"
#include "PortalTag.hpp"

namespace Runes::Portal
{
	struct PortalLEDColour
	{
		uint8_t _red;
		uint8_t _green;
		uint8_t _blue;
	};

	class PortalDriver
	{
	public:
		explicit PortalDriver();
		~PortalDriver();

		HardwareErrorCode Connect();

		void QueueColour(PortalLEDColour colour);
		void QueueColour(uint8_t r, uint8_t g, uint8_t b);

	private:
		enum DriverState
		{
			kDriverStateNotConnected,
			kDriverStateReadyBegin,
			kDriverStateReadyPending,
			kDriverStateActivationBegin,
			kDriverStateActivationPending,
			kDriverStateIdle,
		};

		void                           PortalThread();

		HardwareErrorCode              ProcessRead(uint8_t writeBuffer[0x20], uint8_t* writeBufferLen);

		HardwareErrorCode              ProcessColour(uint8_t writeBuffer[0x20], uint8_t* writeBufferLen);

		// Do NOT expose this at all, this must belong to the portal thread,
		// The only time the main thread can interact with it is to set up
		// the connection.
		HardwareInterface*             _interface;
		std::atomic<DriverState>       _state;

		// MUST rely on atomics here, no mutexes, mutexes only serve to slow
		// things down by their very nature.
		std::atomic<PortalLEDColour>   _colour;

		// Ensure this is after _interface, as this ensures it's destroyed before _interface
		std::thread                    _thread;
		uint8_t                        _timeoutCounter;

		uint8_t                        _version[4];

		uint8_t                        _lastStatusId;
		std::array<Runes::PortalTag, 16> _tags;
	};
}

#endif // RUNES_HARDWARE_PORTAL_DRIVER_HPP