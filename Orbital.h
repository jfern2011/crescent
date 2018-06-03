#pragma once

#include <vector>

#include "EphemerisObject.h"
#include "Event.h"
#include "SharedData.h"

class Orbital : public Event
{
	struct SharedIDs
	{
		SharedIDs(int _object_id = -1, int _mass_id = -1)
			: object_id(_object_id),
			  mass_id(_mass_id)
		{
		}

		int object_id;
		int mass_id;
	};

public:

	Orbital();

	~Orbital();

	bool init(Handle<DataDirectory> shared,
		      const std::string& masses_config);

	int64 dispatch(int64 t_now);

	bool exists(const std::string& name) const;

private:

	bool _init_shared();

	bool _read_masses_config(const std::string& name);


	Handle<DataDirectory> _data;

	std::vector<SharedIDs>
		_ids;

	bool _is_init;

	std::map<std::string, double>
		_name2mass;

	Handle<DataDirectory>
		_telemetry;
};
