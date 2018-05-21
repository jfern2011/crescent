#include "Event.h"

Event::Event(const std::string& name)
	: _name(name)
{
}

Event::~Event()
{
}

std::string Event::get_name() const
{
	return _name;
}

/**
 * Determine if this event should run based
 * on the current time
 */
bool Event::is_due(int64 time) const
{
	return true;
}
