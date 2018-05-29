#pragma once

#include <vector>

#include "Event.h"
#include "RigidBody.h"
#include "SharedData.h"

class Orbital : public Event
{

public:

	Orbital();

	~Orbital();

	bool init(Handle<DataDirectory> shared,
		      const std::string& masses_config,
			  const std::string& ephem_config);

	int64 dispatch(int64 t_now);

	int exists(const std::string& name) const;

private:

	bool _init_shared(Handle< DataDirectory > shared);

	bool _read_ephem_config(const std::string& name);

	bool _read_masses_config(const std::string& name);

	std::vector<RigidBody>
		_bodies;

	Handle<DataDirectory> _data;

	bool _is_init;
};
