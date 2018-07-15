#include "TimeKeeper.h"

namespace Crescent
{
	/**
	 * Constructor
	 */
	TimeKeeper::TimeKeeper()
		: Event("SimulationTime"),
		_directory(), _is_init(false), _t_sim_id(-1)
	{
	}

	/**
	 * Destructor
	 */
	TimeKeeper::~TimeKeeper()
	{
	}

	/**
	 * Update the current simulation time
	 *
	 * @param[in] t_now The current event cycle time
	 *
	 * @return Zero on success
	 */
	int64 TimeKeeper::dispatch(int64 t_now)
	{
		AbortIfNot_2(_is_init, -1);

		_directory->load<double>(_t_sim_id) = t_now * t_step;
		return 0;
	}

	/**
	 * Initialize
	 *
	 * @param[in] dir The directory in which to write the
	 *                current time
	 *
	 * @return True on success
	 */
	bool TimeKeeper::init(Handle<DataDirectory> dir)
	{
		AbortIfNot_2(dir, false);
		_directory = dir;

		_t_sim_id =
			_directory->create_element<double>("sim_time");

		AbortIf_2(_t_sim_id < 0, false);

		_is_init = true;
		return true;
	}
}
