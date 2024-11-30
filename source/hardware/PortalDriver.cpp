/*
	File:
		PortalDriver.cpp

	Description:
		Instance of a connection to a portal.
*/

#include "hardware/PortalDriver.hpp"

#include "RunesDebug.hpp"
#include "hardware/HardwareInterface.hpp"
#include "hardware/WinHidUsbInterface.hpp"

#include <chrono>
#include <unistd.h>

using namespace Runes::Portal;


// How often the portal should tick in microseconds
static constexpr uint32_t TickPeriod = 20000u;


//=============================================================================
// Constructor for PortalDriver
//=============================================================================
PortalDriver::PortalDriver()
: _interface(nullptr)
, _colour({0, 0, 0})
, _thread()
, _timeoutCounter(0)
{
}



//=============================================================================
// Destructor for PortalDriver
//=============================================================================
PortalDriver::~PortalDriver()
{
	if (_interface != nullptr)
	{
		delete _interface;
	}
}



//=============================================================================
// Connect: Connects to a portal
//=============================================================================
HardwareErrorCode PortalDriver::Connect()
{
	_interface = new WinHidUsbInterface();
	HardwareErrorCode error = _interface->connect(PortalType::PORTAL_TYPE_DEFAULT);

	if (error == kHWErrNoError)
	{
		// Kick off the thread
		_thread = std::thread(PortalThread, this);
	}
	else
	{
		delete _interface;
		_interface = nullptr;
	}

	return error;
}



//=============================================================================
// QueueColour: Queues a colour to be sent to the portal
//=============================================================================
void PortalDriver::QueueColour(PortalLEDColour colour)
{
	_colour.store(colour);
}



//=============================================================================
// QueueColour: Queues a colour to be sent to the portal
//=============================================================================
void PortalDriver::QueueColour(uint8_t r, uint8_t g, uint8_t b)
{
	_colour = { r, g, b };
}



//=============================================================================
// PortalThread: The main loop for the portal, runs until the portal's disconnected
//=============================================================================
void PortalDriver::PortalThread()
{
	while(_interface->connected())
	{
		const auto start = std::chrono::steady_clock::now();

		HardwareErrorCode error;

		error = ProcessRead();
		if (error != kHWErrNoError)
		{
			if (error == kHWErrReadTimedOut)
			{
				_timeoutCounter++;
				if (_timeoutCounter == 3)
				{
					_interface->disconnect();
				}
			}
			continue;
		}
		_timeoutCounter = 0;

		error = ProcessColour();
		if (error != kHWErrNoError)
		{
			continue;
		}

		const auto end = std::chrono::steady_clock::now();

		const std::chrono::microseconds delta = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		if (delta.count() < TickPeriod)
		{
			usleep(TickPeriod - delta.count());
		}
		else
		{
			RUNES_LOG_WARN("Portal thread took %ld longer than expected!!", delta.count() - TickPeriod);
		}
	}

	delete _interface;
	_interface = nullptr;
}



//=============================================================================
// ProcessRead: Handle reads from the portal
//=============================================================================
HardwareErrorCode PortalDriver::ProcessRead()
{
	uint8_t readBuffer[0x20];
	HardwareErrorCode error = _interface->readIn(readBuffer, sizeof(readBuffer));
	if (error != kHWErrNoError)
	{
		return error;
	}
	_timeoutCounter = 0;
}



//=============================================================================
// ProcessColour: Write out the current colour
//=============================================================================
HardwareErrorCode PortalDriver::ProcessColour()
{
	PortalLEDColour colour = _colour.load();

	uint8_t writeBuffer[0x20]{};
	writeBuffer[0] = 'C';
	writeBuffer[1] = colour._red;
	writeBuffer[2] = colour._green;
	writeBuffer[3] = colour._blue;

	return _interface->writeOut(writeBuffer, sizeof(writeBuffer));
}