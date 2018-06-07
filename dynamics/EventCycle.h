#pragma once

#include "Event.h"

class EventCycle
{

public:

	EventCycle();

	~EventCycle();

	bool register_event(Handle<Event> event);

	bool run(int64 t_stop);

private:

	int64 _100Hz_count;

	std::vector< Handle<Event> >
		_events;
};
