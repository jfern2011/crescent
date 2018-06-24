#pragma once

#include <string>

#include "crescent.h"

/**
 * Represents a discrete time event
 */
class Event
{

public:

	Event(const std::string& name);

	virtual ~Event();

	virtual int64 dispatch(int64 t_now) = 0;

	std::string get_name() const;

	bool is_due(int64 time) const;

private:

	/**
	 * The name of this event
	 */
	std::string _name;
};
