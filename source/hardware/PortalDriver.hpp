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
#include <queue>
#include <variant>
#include <mutex>

#include "HardwareInterface.hpp"
#include "PortalTag.hpp"
#include "Events.hpp"

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

		void Pump();

		void QueueColour(PortalLEDColour colour);
		void QueueColour(uint8_t r, uint8_t g, uint8_t b);
		Event<void, uint8_t>&                    GetTagPlacedEvent()       { return _tagPlacedEvent; }
		Event<void, uint8_t>&                    GetTagRemovedEvent()      { return _tagRemovedEvent; }
		Event<void, uint8_t, Runes::PortalTag&>& GetTagReadFinishedEvent() { return _tagReadFinishedEvent; }

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

		enum EEventType
		{
			kEventTypeNone,
			kEventTypeFigurePlaced,
			kEventTypeFigureReadComplete,
			kEventTypeFigureRemoved
		};
		struct QueuedEvent
		{
			QueuedEvent(EEventType type)
			: _type(type)
			{
			}
			EEventType _type;
		};
		template <EEventType eventType>
		struct QueuedEventDataByte : public QueuedEvent
		{
			QueuedEventDataByte(uint8_t data)
			: QueuedEvent(eventType)
			, _data(data)
			{
			}

			uint8_t _data;
		};
		typedef QueuedEventDataByte<kEventTypeFigurePlaced>       QueuedEventFigurePlaced;
		typedef QueuedEventDataByte<kEventTypeFigureReadComplete> QueuedEventFigureReadComplete;
		typedef QueuedEventDataByte<kEventTypeFigureRemoved>      QueuedEventFigureRemoved;


		void                           PortalThread();

		HardwareErrorCode              ProcessRead(uint8_t writeBuffer[0x20], uint8_t* writeBufferLen);

		HardwareErrorCode              ProcessColour(uint8_t writeBuffer[0x20], uint8_t* writeBufferLen);

		void                           QueueEvent(QueuedEvent* event);

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

		Event<void, uint8_t>                    _tagPlacedEvent;
		Event<void, uint8_t>                    _tagRemovedEvent;
		Event<void, uint8_t, Runes::PortalTag&> _tagReadFinishedEvent;

		std::mutex                      _eventQueueMutex;
		std::queue<QueuedEvent*>        _eventQueue;
	};
}

#endif // RUNES_HARDWARE_PORTAL_DRIVER_HPP