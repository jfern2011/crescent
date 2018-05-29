#include "abort.h"
#include "Simulation.h"

Simulation::Simulation() : _is_init(false)
{
}

Simulation::~Simulation()
{
}

bool Simulation::create_shared_data()
{
	AbortIf_2(_is_init, false);

	shared.reset(new SharedData());
	AbortIfNot_2(shared, false);

	return true;
}

bool Simulation::init()
{
	AbortIfNot_2(create_shared_data(), false);

	_is_init = true;
	return true;
}
