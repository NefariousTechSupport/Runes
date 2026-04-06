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
#include "LibUsbInterface.hpp"
#include "PortalAlgos.hpp"

#include <chrono>

using namespace Runes::Portal;



//=============================================================================
// Constructor for PortalDriver
//=============================================================================
PortalDriver::PortalDriver()
: _interface(nullptr)
, _state(kDriverStateNotConnected)
, _colourMutex()
, _colour()
, _colourDirty(true)
, _thread()
, _errorCounter(0)
, _version()
, _lastStatusId(0)
, _tags()
, _tagPlacedEvent()
, _tagRemovedEvent()
, _tagReadFinishedEvent()
, _tagReadUpdateEvent()
, _tagWriteCompleteEvent()
, _tagWriteCancelledEvent()
, _eventQueueMutex()
, _eventQueue()
, _writeQueue()
{
	for (uint8_t t = 0; t < _tags.size(); t++)
	{
		_tags[t]._rfidTag = new RfidTag(true);
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
// Pump: Main Thread pumping
//=============================================================================
void PortalDriver::Pump()
{
	MainThreadPollDevices();
	MainThreadPumpQueue();
}



//=============================================================================
// MainThreadPollDevices: Poll for new devices
//=============================================================================
void PortalDriver::MainThreadPollDevices()
{
	if (_state.load() != kDriverStateNotConnected)
	{
		// No need to poll if we're connected
		return;
	}

	if (_interface)
	{
		// Send tag removed events
		for (int i = 0; i < _tags.size(); i++)
		{
			if (_tags[i]._rfidTag->PortalBlocksFilled() != -1)
			{
				_tagRemovedEvent.Invoke(i);
			}
		}

		delete _interface;
		_interface = nullptr;
	}

	if (!_interface)
	{
		_interface = HidUsbInterface::poll();
	}
	if (!_interface)
	{
		_interface = LibUsbInterface::poll();
	}

	if (_interface)
	{
		_state.store(kDriverStateReadyBegin);
		// Kick off the thread
		_thread = std::thread(&PortalDriver::PortalThread, this);
	}
}



//=============================================================================
// MainThreadPumpQueue: dispatch events on the main thread
//=============================================================================
void PortalDriver::MainThreadPumpQueue()
{
	std::queue<QueuedEvent*> queueCopy;

	_eventQueueMutex.lock();
	if (!_eventQueue.empty())
	{
		queueCopy = _eventQueue;
		// Clear the queue
		_eventQueue = std::queue<QueuedEvent*>();
	}
	_eventQueueMutex.unlock();

	// process the copy
	while (!queueCopy.empty())
	{
		QueuedEvent* event = queueCopy.front();
		queueCopy.pop();

		switch(event->_type)
		{
			case kEventTypeNone:
				break;
			case kEventTypeFigurePlaced:
			{
				QueuedEventFigurePlaced* fpEvent = static_cast<QueuedEventFigurePlaced*>(event);
				_tagPlacedEvent.Invoke(fpEvent->_data);
				delete fpEvent;
				break;
			}
			case kEventTypeFigureReadComplete:
			{
				QueuedEventFigureReadComplete* frcEvent = static_cast<QueuedEventFigureReadComplete*>(event);
				PortalTag& tag = _tags[frcEvent->_data];
				tag.StoreHeader();
				tag.StoreMagicMoment();
				tag.StoreRemainingData();
				_tagReadFinishedEvent.Invoke(frcEvent->_data, tag);
				delete frcEvent;
				break;
			}
			case kEventTypeFigureRemoved:
			{
				QueuedEventFigureRemoved* frEvent = static_cast<QueuedEventFigureRemoved*>(event);
				_tagRemovedEvent.Invoke(frEvent->_data);
				delete frEvent;
				break;
			}
			case kEventTypeFigureReadUpdate:
			{
				QueuedEventFigureReadUpdate* fruEvent = static_cast<QueuedEventFigureReadUpdate*>(event);
				_tagReadUpdateEvent.Invoke(fruEvent->_figureId, fruEvent->_progress);
				delete fruEvent;
				break;
			}
			case kEventTypeFigureWriteComplete:
			{
				QueuedEventFigureWriteComplete* fwcEvent = static_cast<QueuedEventFigureWriteComplete*>(event);
				_tagWriteCompleteEvent.Invoke(fwcEvent->_data);
				delete fwcEvent;
				break;
			}
			case kEventTypeFigureWriteCancelled:
			{
				QueuedEventFigureWriteCancelled* fwcEvent = static_cast<QueuedEventFigureWriteCancelled*>(event);
				_tagWriteCancelledEvent.Invoke(fwcEvent->_data);
				delete fwcEvent;
				break;
			}
		}
	}
}



//=============================================================================
// QueueColour: Queues a colour to be sent to the portal
//=============================================================================
void PortalDriver::QueueColour(PortalLEDColour colour)
{
	_colourMutex.lock();

	_colour = colour;
	_colourDirty = true;

	_colourMutex.unlock();
}



//=============================================================================
// QueueColour: Queues a colour to be sent to the portal
//=============================================================================
void PortalDriver::QueueColour(uint8_t r, uint8_t g, uint8_t b)
{
	_colourMutex.lock();

	_colour = { r, g, b };
	_colourDirty = true;

	_colourMutex.unlock();
}



//=============================================================================
// QueueWrite: Queues the figure data to be written out
//=============================================================================
bool PortalDriver::QueueWrite(int index)
{
	RUNES_ASSERT(index < 16);
	if (index >= 16)
	{
		return false;
	}

	Runes::PortalTag& tag = _tags[index];

	bool success = false;

	if (tag.isCore())
	{
		// Flip the active regions cos we're writing
		uint8_t active0 = tag._rfidTag->DetermineActiveDataRegion0() == 0 ? 0x24 : 0x08;
		uint8_t active1 = tag._rfidTag->DetermineActiveDataRegion1() == 0 ? 0x1D : 0x11;

		uint8_t dTagData[sizeof(PortalTagData)];
		uint8_t eTagData[sizeof(PortalTagData)];
		tag._rfidTag->CopyBlocks(&dTagData[0 * BLOCK_SIZE], active0, 7);
		tag._rfidTag->CopyBlocks(&dTagData[7 * BLOCK_SIZE], active1, 4);

		// so much simpler to just have an array here
		std::array<uint8_t, sizeof(dTagData) / BLOCK_SIZE> blockIds =
		{
			/* 08 24 */ active0 + 0,
			/* 09 25 */ active0 + 1,
			/* 0A 26 */ active0 + 2,
			/* 0C 28 */ active0 + 4,
			/* 0D 29 */ active0 + 5,
			/* 0E 2A */ active0 + 6,
			/* 10 2C */ active0 + 8,
			/* 11 2D */ active1 + 0,
			/* 12 2E */ active1 + 1,
			/* 14 30 */ active1 + 3,
			/* 15 31 */ active1 + 4
		};

		static_assert(sizeof(dTagData)      == (blockIds.size() * BLOCK_SIZE));
		static_assert(sizeof(eTagData)      == (blockIds.size() * BLOCK_SIZE));
		static_assert(sizeof(PortalTagData) == (blockIds.size() * BLOCK_SIZE));

		// encrypt
		for (uint8_t i = 0; i < blockIds.size(); i++)
		{
			Runes::encryptBlock(&tag._tagHeader, &dTagData[i * BLOCK_SIZE], &eTagData[i * BLOCK_SIZE], blockIds[i]);
		}

		_writeQueueMutex.lock();

		// enqueue
		uint8_t block[BLOCK_SIZE];
		for (uint8_t i = 0; i < blockIds.size(); i++)
		{
			memcpy(block, &eTagData[i * BLOCK_SIZE], BLOCK_SIZE);
			_writeQueue.emplace_back(index, blockIds[i], block);
		}

		_writeQueueMutex.unlock();

		success = true;
	}

	return success;
}



//=============================================================================
// PortalThread: The main loop for the portal, runs until the portal's disconnected
//=============================================================================
void PortalDriver::PortalThread()
{
	_errorCounter = 0;

	while(_interface->connected())
	{
		uint8_t writeBuffer[0x20];
		uint8_t writeBufferLen = 0;

		HardwareErrorCode error;

		error = ProcessRead(writeBuffer, &writeBufferLen);
		if (error != kHWErrNoError)
		{
			_errorCounter++;
			if (_errorCounter == 3)
			{
				_interface->disconnect();
			}
			continue;
		}
		_errorCounter = 0;

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
			if (_interface->getPortalType() == PORTAL_TYPE_XBOX360)
			{
				uint8_t xbox360WriteBuffer[sizeof(writeBuffer)];
				size_t realWriteBufferLen = std::min<size_t>(writeBufferLen + 2, sizeof(xbox360WriteBuffer) - HardwareInterface::Xbox360BufferHeaderSize);

				xbox360WriteBuffer[0] = 0x0B;
				xbox360WriteBuffer[1] = 0x14;
				memcpy(&xbox360WriteBuffer[HardwareInterface::Xbox360BufferHeaderSize], writeBuffer, realWriteBufferLen);
				_interface->writeOut(xbox360WriteBuffer, realWriteBufferLen);
			}
			else
			{
				_interface->writeOut(writeBuffer, writeBufferLen);
			}
		}
	}

	_thread.detach();
	_state.store(kDriverStateNotConnected);
}



//=============================================================================
// ProcessRead: Handle reads from the portal
//=============================================================================
HardwareErrorCode PortalDriver::ProcessRead(uint8_t writeBuffer[HardwareInterface::EP0WriteSize], uint8_t* writeBufferLen)
{
	uint8_t readBuffer[HardwareInterface::EP0ReadSize] = {};
	HardwareErrorCode error = kHWErrNoError;

	if (_interface->getPortalType() == PORTAL_TYPE_XBOX360)
	{
		uint8_t xbox360ReadBuffer[HardwareInterface::EP0ReadSize] = {};
		error = _interface->readIn(xbox360ReadBuffer, sizeof(xbox360ReadBuffer));
		if (error != kHWErrNoError)
		{
			return error;
		}

		if (xbox360ReadBuffer[0x00] == 0x0B
		 && xbox360ReadBuffer[0x01] == 0x14)
		{
			memcpy(&readBuffer[0], &xbox360ReadBuffer[HardwareInterface::Xbox360BufferHeaderSize], sizeof(readBuffer));
		}
		else
		{
			// Not something we care about
			return error;
		}
	}
	else
	{
		error = _interface->readIn(readBuffer, sizeof(readBuffer));
		if (error != kHWErrNoError)
		{
			return error;
		}
	}


	_errorCounter = 0;

	switch (_state.load())
	{
		case kDriverStateNotConnected:
			error = kHWErrLostConnection;
			break;

		case kDriverStateActivationReset:
			writeBuffer[0] = 'A';
			writeBuffer[1] = 0x00;
			*writeBufferLen = 2;
			_state.store(kDriverStateReadyBegin);
			break;
		case kDriverStateReadyBegin:
			writeBuffer[0] = 'R';
			*writeBufferLen = 1;
			_state.store(kDriverStateReadyPending);
			break;

		case kDriverStateReadyPending:
			if (readBuffer[0] == 'S')
			{
				RUNES_LOG_WARN("Expected R packet but got S packet, we'll keep waiting");
				// Turn off that annoying antenna
				_state.store(kDriverStateActivationReset);
				break;
			}
			if (readBuffer[0] != 'R')
			{
				RUNES_LOG_WARN("Expected R packet but got {}, restarting the process", readBuffer[0]);
				// revert back
				_state.store(kDriverStateReadyBegin);
				break;
			}
			_version[0] = readBuffer[1];
			_version[1] = readBuffer[2];
			_version[2] = readBuffer[3];
			_version[3] = readBuffer[4];

			[[fallthrough]];
		case kDriverStateActivationBegin:
			writeBuffer[0] = 'A';
			writeBuffer[1] = 0x01;
			*writeBufferLen = 2;
			_state.store(kDriverStateActivationPending);
			break;

		case kDriverStateActivationPending:
			if (readBuffer[0] != 'A')
			{
				RUNES_LOG_WARN("Expected A packet but got {}, restarting the process", readBuffer[0]);
				// revert back
				_state.store(kDriverStateActivationBegin);
				break;
			}
			// idk how this response data is structured
			_state.store(kDriverStateIdle);
			break;

		case kDriverStateIdle:
			{
				_writeQueueMutex.lock();
				bool processedQuery = false;
				int8_t sendWriteComplete = -1;

				if (!_writeQueue.empty())
				{
					WriteCmd& cmd = _writeQueue.front();

					switch (cmd._writeStage)
					{
						case 0:
							// send write request
							writeBuffer[0] = 'W';
							writeBuffer[1] = cmd._figure;
							writeBuffer[2] = cmd._block;
							memcpy(&writeBuffer[3], cmd._data, BLOCK_SIZE);

							*writeBufferLen = BLOCK_SIZE + 3;

							// next step is waiting for write response
							cmd._writeStage = 1;
							break;
						case 1:
							if (readBuffer[0] == 'W'
							 && (readBuffer[1] & 0xF) == cmd._figure
							 && readBuffer[2] == cmd._block)
							{
								writeBuffer[0] = 'Q';
								writeBuffer[1] = cmd._figure;
								writeBuffer[2] = cmd._block;
								*writeBufferLen = 3;

								// next step is waiting for query response (validation)
								cmd._writeStage = 2;
							}
							break;
						case 2:
							if (readBuffer[0] == 'Q'
							 && (readBuffer[1] & 0x0F) == cmd._figure
							 && readBuffer[2] == cmd._block)
							{
								processedQuery = true;

								// Verify the request was successful
								if (readBuffer[1] & 0x10 && memcmp(cmd._data, &readBuffer[3], BLOCK_SIZE) == 0)
								{
									// We're done here, it's been written and verified to be what we wanted
									_writeQueue.pop_front();

									// defer this so we don't lock both mutexes at once
									sendWriteComplete = cmd._figure;
								}
								else
								{
									// Reset
									cmd._writeStage = 0;
								}
							}
							break;
					}
				}

				_writeQueueMutex.unlock();

				if (sendWriteComplete >= 0)
				{
					// We send the event here to avoid locking multiple mutexes at once
					QueueEvent(new QueuedEventFigureWriteComplete(sendWriteComplete));
				}

				if (!processedQuery && readBuffer[0] == 'Q')
				{
					uint8_t requestedFigure = readBuffer[1] & 0xF;
					bool readFigure = readBuffer[1] & 0x10;
					uint8_t requestedBlock  = readBuffer[2];
					RfidTag* rfidTag = _tags[requestedFigure]._rfidTag;
					if (readFigure && requestedBlock == rfidTag->PortalBlocksFilled())
					{
						rfidTag->PortalFillBlock(&readBuffer[3]);

						QueueEvent(new QueuedEventFigureReadUpdate(requestedFigure, rfidTag->PortalBlocksFilled()));

						uint8_t blockToRead = rfidTag->PortalBlocksFilled();
						bool shouldRequest = rfidTag->PortalBlocksRequested() == blockToRead;

						if (!rfidTag->PortalFinishedRead()
							&& shouldRequest)
						{
							rfidTag->PortalMarkBlockRequested(blockToRead);
							writeBuffer[0] = 'Q';
							writeBuffer[1] = requestedFigure;
							writeBuffer[2] = blockToRead;
							*writeBufferLen = 3;
						}
					}
					else if (!readFigure)
					{
						rfidTag->PortalCancelBlockRequest(requestedBlock);
					}

					if (rfidTag->PortalFinishedRead())
					{
						QueueEvent(new QueuedEventFigureReadComplete(requestedFigure));
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
								break;
							case 0b10:
								{
									_tags[s]._rfidTag->PortalClearData();

									_writeQueueMutex.lock();

									uint8_t oldQueueLen = _writeQueue.size();
									_writeQueue.remove_if([s](WriteCmd& cmd){ return cmd._figure == s; });
									bool sendWriteCancelled = oldQueueLen != _writeQueue.size();

									_writeQueueMutex.unlock();

									// Defer the event to prevent locking both mutexes at once
									if (sendWriteCancelled)
									{
										QueueEvent(new QueuedEventFigureWriteCancelled(s));
									}
									QueueEvent(new QueuedEventFigureRemoved(s));
								}
								break;
							case 0b11:
								{
									_tags[s]._rfidTag->PortalPrepareRead();
									QueueEvent(new QueuedEventFigurePlaced(s));

									Runes::RfidTag* rfidTag = _tags[s]._rfidTag;
									rfidTag->PortalMarkBlockRequested(0);

									// Read block 0
									writeBuffer[0] = 'Q';
									writeBuffer[1] = s;
									writeBuffer[2] = 0;
									*writeBufferLen = 3;
								}
								break;
						}
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
HardwareErrorCode PortalDriver::ProcessColour(uint8_t writeBuffer[HardwareInterface::EP0WriteSize], uint8_t* writeBufferLen)
{
	_colourMutex.lock();

	PortalLEDColour colour = _colour;
	bool wasDirty = std::exchange(_colourDirty, false);

	_colourMutex.unlock();

	if (wasDirty)
	{
		writeBuffer[0] = 'C';
		writeBuffer[1] = colour._red | 0xFF;
		writeBuffer[2] = colour._green;
		writeBuffer[3] = colour._blue;
		*writeBufferLen = 4;
	}

	return kHWErrNoError;
}



void PortalDriver::QueueEvent(QueuedEvent* event)
{
	_eventQueueMutex.lock();
	_eventQueue.push(event);
	_eventQueueMutex.unlock();
}
