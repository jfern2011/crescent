#include <Windows.h>

#include "EventCycle.h"
#include "Verbosity.h"

/**
 * Constructor
 *
 * @param[in] realtime If true, run in real-time
 */
EventCycle::EventCycle(bool realtime)
	: _100Hz_count(0), _events(), _progress(), _realtime(realtime)
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
	_progress.reset(t_stop);

	if (_realtime && Verbosity::level >= terse)
	{
		std::printf("Realtime mode enabled.\n");
		std::fflush(stdout);
	}

	for (; _100Hz_count <= t_stop; _100Hz_count++)
	{
		_progress.update(_100Hz_count);

		for (size_t i = 0; i < _events.size(); i++)
		{
			AbortIf_2(_events[i]->dispatch(_100Hz_count)
				< 0, false);
		}

		/**
		 * Sleep for 10 milliseconds
		 */
		if (_realtime) Sleep(10);
	}

	return true;
}

/**
 * Constructor
 *
 * @param[in] end The end value, which corresponds to
 *                completion
 */
EventCycle::Progress::Progress()
	: _end(0), _percent(0), _prev_percent(0)
{
}

/**
 * Destructor
 */
EventCycle::Progress::~Progress()
{
}

/**
 * Initialize
 *
 * @param[in] end The end value, which corresponds to
 *                100% completion
 *
 * @return True on success
 */
void EventCycle::Progress::reset(int64 end)
{
	_end = end;
}

/**
 * Display the current progress. Requires Verbosity of
 * at least terse
 *
 * @param[in] now The current 100Hz time step
 */
void EventCycle::Progress::update(int64 now)
{
	if (_end > 0) _percent = now * 100 / _end;

	if (Verbosity::level < terse)
		return;

	if (_percent > _prev_percent)
	{
		std::printf( "Running... %lld%%   \r",
			_percent);

		_prev_percent = _percent;
		std::fflush(stdout);
	}
}
