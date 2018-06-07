#include "EventCycle.h"

EventCycle::EventCycle() : _100Hz_count(0), _events()
{
}

EventCycle::~EventCycle()
{
}

bool EventCycle::register_event(Handle<Event> event)
{
	AbortIfNot_2(event, false);

	_events.push_back(event);
	return true;
}

bool EventCycle::run(int64 t_stop)
{
	for (; _100Hz_count <= t_stop; _100Hz_count++)
	{
		for (size_t i = 0; i < _events.size(); i++)
		{
			AbortIf_2(_events[i]->dispatch(_100Hz_count)
				< 0, false);
		}
	}

	return true;
}
