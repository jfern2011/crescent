#pragma once

#include <string>

#include "Vector.h"

/**
 * Represents a rigid body within a larger system
 */
struct RigidBody
{
	Vector<3>   accel;
	double      mass;
	std::string name;
	Vector<6>   state;
};
