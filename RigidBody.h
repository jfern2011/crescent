#pragma once

#include <string>
#include <vector>

#include "Vector.h"

/**
 * Represents a rigid body within a larger system
 */
struct RigidBody
{
	RigidBody(const std::string& _name, double _mass = 0.0)
		: name(_name),
		  mass(_mass),
		  accel_id(3),
		  state_id(3)
	{
	}

	Vector<3>   accel;
	double      mass;
	std::string name;
	Vector<6>   state;

	/* SharedData IDs */

	std::vector<int>
		accel_id;

	int mass_id;
	int name_id;

	std::vector<int>
		state_id;
};
