#include "EventCycle.h"

EventCycle::EventCycle() : _events()
{
}

EventCycle::~EventCycle()
{
}

bool EventCycle::register_event(Handle<Event> event)
{
	return true;
}

bool EventCycle::run(int64 t_stop)
{
	return true;
}
