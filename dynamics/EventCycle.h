#pragma once

#include "Event.h"

/**
 * Loops through a list of registered Events until the simulation end
 * time, dispatching each to perform its specific task(s) 
 */
class EventCycle
{

public:

	EventCycle();

	~EventCycle();

	bool register_event(Handle<Event> event);

	bool run(int64 t_stop);

private:

	/**
	 * The current 100Hz time step
	 */
	int64 _100Hz_count;

	/**
	 * The events to dispatch
	 */
	std::vector< Handle<Event> >
		_events;
};
