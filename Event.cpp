#include "Event.h"

namespace Crescent
{
	/**
	 * Constructor
	 *
	 * @param[in] name The name of this event
	 */
	Event::Event(const std::string& name)
		: _name(name)
	{
	}

	/**
	 * Destructor
	 */
	Event::~Event()
	{
	}

	/**
	 * Get the name of this event
	 *
	 * @return The event name
	 */
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
}
