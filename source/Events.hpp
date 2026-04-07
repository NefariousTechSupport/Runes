/*
	File:
		Events.hpp

	Description:
		Handles event dispatch.
*/

#ifndef RUNES_EVENTS_HPP
#define RUNES_EVENTS_HPP

#include <cstdint>
#include <functional>
#include <limits>
#include <map>

namespace Runes
{
	typedef uint32_t EventListenerID;
	constexpr EventListenerID kInvalidEventListenerID = std::numeric_limits<EventListenerID>::max();

	template<typename ReturnType, typename... ParamTypes>
	struct Event
	{
	private:
		using FunctionType = std::function<ReturnType (ParamTypes...)>;
		using MapType = std::map<EventListenerID, FunctionType>;

	public:
		Event()
		: _listeners()
		, _currentId(0)
		{
		}

		EventListenerID AddListener(FunctionType listener)
		{
			_listeners.insert(std::pair<EventListenerID, FunctionType>(++_currentId, listener));
			return _currentId;
		}

		void RemoveListener(EventListenerID id)
		{
			_listeners.erase(id);
		}

		void Invoke(ParamTypes... parameters)
		{
			MapType copy = _listeners;
			typename MapType::iterator iter;
			for (iter = copy.begin(); iter != copy.end(); iter++)
			{
				iter->second(std::forward<ParamTypes>(parameters)...);
			}
		}
	private:
		MapType _listeners;
		EventListenerID _currentId;
	};
}

#endif // RUNES_EVENTS_HPP