#pragma once

#include <string>
#include <vector>

#include "Vector.h"

/**
 * Represents an orbiting (e.g. celestial) body
 */
struct EphemerisObject
{
	/**
	 * Constructor
	 *
	 * @param[in] _name The name of this object
	 * @param[in] _mass The mass of this object
	 */
	EphemerisObject(const std::string& _name = "", double _mass = 0.0)
		: name(_name),
		  mass(_mass)
	{
	}

	/**
	 * The acceleration of this object, meters,
	 * ECI J2000
	 */
	Vector<3>   accel;

	/**
	 * The mass of this object, kilograms
	 */
	double      mass;

	/**
	 * The name of this object
	 */
	std::string name;

	/**
	 * The object's ephemeris, meters, ECI
	 * J2000
	 */
	Vector<6>   rv_eci;
};
