#pragma once

#include "Event.h"

/**
 * Loops through a list of registered Events until the simulation end
 * time, dispatching each to perform its specific task(s) 
 */
class EventCycle
{

public:

	/**
	 * Updates the EventCycle progress
	 */
	class Progress
	{

	public:

		Progress();

		~Progress();

		void reset (int64 end);

		void update(int64 now);

	private:

		/**
		 * When reached => finished
		 */
		int64 _end;

		/**
		 * Percent complete
		 */
		int64 _percent;

		/**
		 * Previous percent complete
		 */
		int64 _prev_percent;
	};

	EventCycle(bool realtime);

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

	/**
	 * Updates our progress
	 */
	Progress _progress;

	/**
	 * If true, run in real-time
	 */
	bool _realtime;
};
