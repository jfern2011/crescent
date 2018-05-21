#pragma once

#include <vector>

#include "Event.h"
#include "RigidBody.h"

class Orbital : public Event
{

public:

	Orbital();

	~Orbital();

	int64 dispatch(int64 t_now);

private:

	std::vector<RigidBody>
		_bodies;
};
