#pragma once

#include <string>
#include <vector>

#include "Vector.h"

/**
 * Represents an orbiting (e.g. celestial) body
 */
struct EphemerisObject
{
	EphemerisObject(const std::string& _name = "", double _mass = 0.0)
		: name(_name),
		  mass(_mass)
	{
	}

	Vector<3>   accel;
	double      mass;
	std::string name;
	Vector<3>   r_eci;
	Vector<3>   v_eci;
};
