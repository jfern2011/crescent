#pragma once

#include "SharedData.h"

class Simulation
{

public:

	Simulation();

	~Simulation();

	bool create_shared_data();

	bool init();

	Handle<SharedData>
		shared;

private:

	bool _is_init;
};
