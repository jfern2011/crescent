#pragma once

#include "Event.h"
#include "SharedData.h"

namespace Crescent
{
	/**
	 * Updates the current simulation time
	 */
	class TimeKeeper : public Event
	{

	public:

		/**
		 * The simulation time step
		 */
		const double t_step = 0.01;

		TimeKeeper();

		~TimeKeeper();

		int64 dispatch(int64 t_now);

		bool init(Handle<DataDirectory> dir);

	private:

		/**
		 * Write the current time here
		 */
		Handle<DataDirectory> _directory;

		/**
		 * True if initialized
		 */
		bool _is_init;

		/**
		 * Simulation time (sec), shared ID
		 */
		int _t_sim_id;
	};
}
