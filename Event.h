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

private:

	std::string _name;
};
