#pragma once

#include "dynamics/EventCycle.h"
#include "CommandLine/CommandLine.h"
#include "SharedData.h"

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

	Handle<SharedData>
		shared;

private:

	bool _init_telem(const std::string& config);

	bool _init_time();

	EventCycle _cycle;

	bool _is_init;
};
