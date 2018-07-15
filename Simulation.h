#pragma once

#include "EventCycle.h"
#include "CommandLine/CommandLine.h"
#include "SharedData.h"

namespace Crescent
{
	/**
	 * The top-level simulation class
	 */
	class Simulation
	{

	public:

		Simulation();

		~Simulation();

		bool create_ephemeris(const std::string& ephem_config);

		bool create_orbital(const std::string& masses_config);

		bool create_shared_data();

		bool go(int64 t_stop);

		bool init(const CommandLine& cmd);

		/**
		 * The shared data system
		 */
		Handle<SharedData> shared;

	private:

		bool _init_telem(const std::string& config);

		bool _init_time();

		/**
		 * Repeatedly loops through all events until the end
		 */
		Handle<EventCycle> _cycle;

		/**
		 * True if initialized
		 */
		bool _is_init;
	};
}
