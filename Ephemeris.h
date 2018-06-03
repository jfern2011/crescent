#pragma once

#include "EphemerisObject.h"
#include "Event.h"
#include "SharedData.h"

class EphemerisManager : public Event
{
	struct SharedIDs
	{
		SharedIDs()
			: object_id(-1),
			  a_eci_id(3,-1),
			  r_eci_id(3,-1),
			  v_eci_id(3,-1),
			  mass_id(-1)
		{
		}

		int object_id;

		std::vector<int> a_eci_id;
		std::vector<int> r_eci_id;
		std::vector<int> v_eci_id;

		int mass_id;
	};

public:

	EphemerisManager();

	~EphemerisManager();

	int64 dispatch(int64 t_now);

	bool init(Handle<DataDirectory> shared,
			  const std::string& config);

private:

	bool _init_telemetry();

	bool _update_telemetry();

	std::vector< SharedIDs >
		_ids;

	bool _is_init;

	Handle<DataDirectory>
		_subdir;

	Handle<DataDirectory>
		_telemetry;
};
