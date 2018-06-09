#include "TimeKeeper.h"

TimeKeeper::TimeKeeper()
	: Event("SimulationTime"),
	  _directory(), _is_init(false), _t_sim_id(-1)
{
}

TimeKeeper::~TimeKeeper()
{
}

int64 TimeKeeper::dispatch(int64 t_now)
{
	AbortIfNot_2(_is_init, -1);

	_directory->load<double>(_t_sim_id) = t_now * t_step;
	return 0;
}

bool TimeKeeper::init(Handle<DataDirectory> dir)
{
	AbortIfNot_2(dir, false);
	_directory = dir;

	_t_sim_id = _directory->create_element<double>("t_sim");

	AbortIf_2(_t_sim_id < 0, false);

	_is_init = true;
	return true;
}
