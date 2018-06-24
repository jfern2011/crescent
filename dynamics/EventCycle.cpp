#include "EventCycle.h"

/**
 * Constructor
 */
EventCycle::EventCycle() : _100Hz_count(0), _events()
{
}

/**
 * Destructor
 */
EventCycle::~EventCycle()
{
}

/**
 * Register a new event to be executed on each iteration of
 * the event cycle
 *
 * @param[in] event Register this event
 *
 * @return True on success
 */
bool EventCycle::register_event(Handle<Event> event)
{
	AbortIfNot_2(event, false);

	_events.push_back(event);
	return true;
}

/**
 * Cycle through the events repeatedly until simulation end
 *
 * @param[in] t_stop The end time
 *
 * @return True on success
 */
bool EventCycle::run(int64 t_stop)
{
	int64 percent = 0, prev_percent = 0;

	for (; _100Hz_count <= t_stop; _100Hz_count++)
	{
		if (t_stop > 0)
			percent = _100Hz_count * 100 / t_stop;
		if (percent > prev_percent)
		{
			prev_percent = percent;
			std::printf("Running... %d%%   \r", (int)percent);
			std::fflush(stdout);
		}

		for (size_t i = 0; i < _events.size(); i++)
		{
			AbortIf_2(_events[i]->dispatch(_100Hz_count)
				< 0, false);
		}
	}

	return true;
}
