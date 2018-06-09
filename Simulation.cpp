#include "abort.h"
#include "EphemerisManager.h"
#include "Orbital.h"
#include "Simulation.h"
#include "dynamics/TimeKeeper.h"

Simulation::Simulation() : _cycle(), _is_init(false)
{
}

Simulation::~Simulation()
{
}

bool Simulation::create_ephemeris(const std::string& ephem_config)
{
	Handle<EphemerisManager> manager(new EphemerisManager());
	AbortIfNot_2(manager, false);

	AbortIfNot_2(manager->init(shared->root(), ephem_config),
		false);

	AbortIfNot_2(_cycle.register_event(manager),
		false);

	return true;
}

bool Simulation::create_orbital(const std::string& masses_config)
{
	Handle<Orbital> orbital(new Orbital());
	AbortIfNot_2(orbital, false);

	AbortIfNot_2(orbital->init(shared->root(), masses_config), false);

	AbortIfNot_2(_cycle.register_event(orbital),
		false);

	return true;
}

bool Simulation::create_shared_data()
{
	Handle<DataAccountant> accountant(new DataAccountant());

	shared.reset(new SharedData(accountant));
	AbortIfNot_2(shared, false);

	return true;
}

bool Simulation::go(int64 t_stop)
{
	AbortIfNot_2(_cycle.run(t_stop), false);
	return true;
}

bool Simulation::init(const CommandLine& cmd)
{
	AbortIf_2(_is_init, false);

	AbortIfNot_2(create_shared_data(), false);

	std::string config;
	AbortIfNot_2(cmd.get<std::string>("masses_config", config),
		false);

	AbortIfNot_2(create_orbital(config), false);

	AbortIfNot_2(cmd.get<std::string>( "ephem_config", config),
		false);

	AbortIfNot_2(create_ephemeris(config), false);

	AbortIfNot_2(_init_time(), false);

	_is_init = true;
	return true;
}

bool Simulation::_init_time()
{
	Handle<TimeKeeper> keeper(new TimeKeeper());
	AbortIfNot_2(keeper, false);

	AbortIfNot_2(keeper->init(shared->root()), false);

	AbortIfNot_2(_cycle.register_event(keeper),
		false);

	return true;
}
