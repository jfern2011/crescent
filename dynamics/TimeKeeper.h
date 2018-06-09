#pragma once

#include "Event.h"
#include "SharedData.h"

class TimeKeeper : public Event
{

public:

	const double t_step = 0.01;

	TimeKeeper();

	~TimeKeeper();

	int64 dispatch(int64 t_now);

	bool init(Handle<DataDirectory> dir);

private:

	Handle<DataDirectory> _directory;

	bool _is_init;

	/**
	 * Simulation time (sec), shared ID
	 */
	int _t_sim_id;
};
