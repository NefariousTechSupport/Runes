/*
	File:
		PortalDriver.cpp

	Description:
		Instance of a connection to a portal.
*/

#include "PortalDriver.hpp"

#include "RunesDebug.hpp"
#include "HardwareInterface.hpp"
#include "HidUsbInterface.hpp"

#include <chrono>
#include <unistd.h>

using namespace Runes::Portal;

#if 1
#define RUNES_PORTAL_LOG(fmt, ...) RUNES_LOG_INFO(fmt, ## __VA_ARGS__)
#else
#define RUNES_PORTAL_LOG(fmt, ...)
#endif

// How often the portal should tick in microseconds
static constexpr uint32_t TickPeriod = 20000u;


//=============================================================================
// Constructor for PortalDriver
//=============================================================================
PortalDriver::PortalDriver()
: _interface(nullptr)
, _state(kDriverStateNotConnected)
, _colour({0, 0, 0})
, _thread()
, _timeoutCounter(0)
, _version({0, 0, 0, 0})
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
	if (_state.load() != kDriverStateNotConnected)
	{
		return kHWErrAlreadyConnected;
	}

	_interface = new HidUsbInterface();
	HardwareErrorCode error = _interface->connect(PortalType::PORTAL_TYPE_DEFAULT);

	if (error == kHWErrNoError)
	{
		_state.store(kDriverStateReadyBegin);
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
	_timeoutCounter = 0;

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
			RUNES_LOG_WARN("Portal thread took %ld microseconds longer than expected!!", delta.count() - TickPeriod);
		}
	}

	_thread.detach();
	_state.store(kDriverStateNotConnected);

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

	RUNES_PORTAL_LOG("Received read of %02X %02X %02X %02X %02X %02X", readBuffer[0], readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4], readBuffer[5]);

	uint8_t writeBuffer[0x20] {};

	RUNES_PORTAL_LOG("Processing state %d", _state.load());

	switch (_state.load())
	{
		case kDriverStateNotConnected:
			error = kHWErrLostConnection;
			break;

		case kDriverStateReadyBegin:
			writeBuffer[0] = 'R';
			error = _interface->writeOut(writeBuffer, 0x1);
			_state.store(kDriverStateReadyPending);
			break;

		case kDriverStateReadyPending:
			if (readBuffer[0] == 'S')
			{
				// Sometimes it just starts out with status
				_state.store(kDriverStateIdle);
				break;
			}
			if (readBuffer[0] != 'R')
			{
				// revert back
				_state.store(kDriverStateReadyBegin);
				break;
			}
			_version[0] = readBuffer[1];
			_version[1] = readBuffer[2];
			_version[2] = readBuffer[3];
			_version[3] = readBuffer[4];
			_state.store(kDriverStateActivationBegin);
			break;

		case kDriverStateActivationBegin:
			writeBuffer[0] = 'A';
			writeBuffer[1] = 0x01;
			error = _interface->writeOut(writeBuffer, 0x2);
			_state.store(kDriverStateActivationPending);
			break;

		case kDriverStateActivationPending:
			if (readBuffer[0] == 'S')
			{
				// Sometimes it just starts out with status
				_state.store(kDriverStateIdle);
				break;
			}
			if (readBuffer[0] != 'A')
			{
				// revert back
				_state.store(kDriverStateActivationBegin);
				break;
			}
			_version[0] = readBuffer[1];
			_version[1] = readBuffer[2];
			_version[2] = readBuffer[3];
			_version[3] = readBuffer[4];
			_state.store(kDriverStateIdle);
			break;

		case kDriverStateIdle:
			if (readBuffer[0] == 'S')
			{
				RUNES_PORTAL_LOG("Received status report of %02X %02X %02X %02X %02X", readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4], readBuffer[5]);
			}
			break;
	}

	return error;
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