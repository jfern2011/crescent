#pragma once

#include "EphemerisObject.h"
#include "Event.h"
#include "SharedData.h"

class EphemerisManager : public Event
{
	struct SharedIDs
	{
		SharedIDs() : SharedIDs("")
		{
		}

		SharedIDs(const std::string& _name)
			: object_id(-1),
			  a_eci_id(3,-1),
			  r_eci_id(3,-1),
			  v_eci_id(3,-1),
			  mass_id(-1),
			  name(_name)
		{
		}

		int object_id;

		std::vector<int> a_eci_id;
		std::vector<int> r_eci_id;
		std::vector<int> v_eci_id;

		int mass_id;

		std::string name;

		Handle<DataDirectory>
			telemetry;
	};

public:

	const static int64 period = 2; // 50Hz

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
};
