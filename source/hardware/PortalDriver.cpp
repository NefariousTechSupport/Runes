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
, _colour()
, _thread()
, _timeoutCounter(0)
, _version()
, _lastStatusId(0)
, _tags()
{
	for (uint8_t t = 0; t < _tags.size(); t++)
	{
		_tags[t]._rfidTag = new RfidTag();
	}
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
		_thread = std::thread(&PortalDriver::PortalThread, this);
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
		uint8_t writeBuffer[0x20];
		uint8_t writeBufferLen = 0;

		HardwareErrorCode error;

		error = ProcessRead(writeBuffer, &writeBufferLen);
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

		if (writeBufferLen == 0)
		{
			error = ProcessColour(writeBuffer, &writeBufferLen);
			if (error != kHWErrNoError)
			{
				continue;
			}
		}

		if (writeBufferLen != 0)
		{
			_interface->writeOut(writeBuffer, writeBufferLen);
		}

		const auto end = std::chrono::steady_clock::now();

		const std::chrono::microseconds delta = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		if (delta.count() < TickPeriod)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(TickPeriod - delta.count()));
		}
		else
		{
			RUNES_LOG_WARN("Portal thread took {} microseconds longer than expected!!", delta.count() - TickPeriod);
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
HardwareErrorCode PortalDriver::ProcessRead(uint8_t writeBuffer[0x20], uint8_t* writeBufferLen)
{
	uint8_t readBuffer[0x20];
	HardwareErrorCode error = _interface->readIn(readBuffer, sizeof(readBuffer));
	if (error != kHWErrNoError)
	{
		return error;
	}
	if (readBuffer[0] == 'Q')
	RUNES_PORTAL_LOG("Received read of {:02X} {:02X} {:02X} {:02X} {:02X} {:02X}", readBuffer[0], readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4], readBuffer[5]);
	_timeoutCounter = 0;

	switch (_state.load())
	{
		case kDriverStateNotConnected:
			error = kHWErrLostConnection;
			break;

		case kDriverStateReadyBegin:
			writeBuffer[0] = 'R';
			*writeBufferLen = 1;
			_state.store(kDriverStateReadyPending);
			break;

		case kDriverStateReadyPending:
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
			*writeBufferLen = 2;
			_state.store(kDriverStateActivationPending);
			break;

		case kDriverStateActivationPending:
			if (readBuffer[0] != 'A')
			{
				// revert back
				_state.store(kDriverStateActivationBegin);
				break;
			}
			// idk how this response data is structured
			_state.store(kDriverStateIdle);
			break;

		case kDriverStateIdle:
			if (readBuffer[0] == 'Q')
			{
				uint8_t requestedFigure = readBuffer[1] & 0xF;
				bool readFigure = readBuffer[1] & 0x10;
				uint8_t requestedBlock  = readBuffer[2];
				RfidTag* rfidTag = _tags[requestedFigure]._rfidTag;
				if (readFigure && requestedBlock == rfidTag->PortalBlocksFilled())
				{
					rfidTag->PortalFillBlock(&readBuffer[3]);
				}
				else if (!readFigure)
				{
					rfidTag->PortalCancelBlockRequest(requestedBlock);
				}
			}
			else if (readBuffer[0] == 'S')
			{
				uint32_t status = (readBuffer[4] << 24) | (readBuffer[3] << 16) | (readBuffer[2] << 8) | (readBuffer[1]);
				_lastStatusId = readBuffer[5];

				for (uint8_t s = 0; s < _tags.size(); s++)
				{
					uint8_t statusItem = (status >> (s * 2)) & 0b11;
					switch(statusItem)
					{
						case 0b00:
							break;
						case 0b01:
							{
								Runes::RfidTag* rfidTag = _tags[s]._rfidTag;
								uint8_t blockToRead = rfidTag->PortalBlocksFilled();
								bool shouldRequest = rfidTag->PortalBlocksRequested() == blockToRead;

								if (!rfidTag->PortalFinishedRead()
								 && shouldRequest)
								{
									RUNES_LOG_INFO("block to read is {}; blocks requested is {}; blocks filled is {}", blockToRead, rfidTag->PortalBlocksRequested(), rfidTag->PortalBlocksFilled());
									rfidTag->PortalMarkBlockRequested(blockToRead);
									writeBuffer[0] = 'Q';
									writeBuffer[1] = s;
									writeBuffer[2] = blockToRead;
									*writeBufferLen = 3;
								}
							}
							break;
						case 0b10:
							_tags[s]._rfidTag->PortalClearData();
							break;
						case 0b11:
							_tags[s]._rfidTag->PortalPrepareRead();
							break;
					}
				}
			}
			break;
	}

	return error;
}



//=============================================================================
// ProcessColour: Write out the current colour
//=============================================================================
HardwareErrorCode PortalDriver::ProcessColour(uint8_t writeBuffer[0x20], uint8_t* writeBufferLen)
{
	PortalLEDColour colour = _colour.load();

	writeBuffer[0] = 'C';
	writeBuffer[1] = colour._red;
	writeBuffer[2] = colour._green;
	writeBuffer[3] = colour._blue;
	*writeBufferLen = 4;

	return kHWErrNoError;
}
