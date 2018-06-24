#include "abort.h"
#include "EphemerisManager.h"
#include "Orbital.h"
#include "Simulation.h"
#include "dynamics/Telemetry.h"
#include "dynamics/TimeKeeper.h"

/**
 * Constructor
 */
Simulation::Simulation() : _cycle(), _is_init(false)
{
}

/**
 * Destructor
 */
Simulation::~Simulation()
{
}

/**
 * Create the ephemeris manager component
 *
 * @param[in] ephem_config The ephemeris config file
 *
 * @return True on success
 */
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

/**
 * Create the multi-body system
 *
 * @param[in] masses_config The masses config file
 *
 * @return True on success
 */
bool Simulation::create_orbital(const std::string& masses_config)
{
	Handle<Orbital> orbital(new Orbital());
	AbortIfNot_2(orbital, false);

	AbortIfNot_2(orbital->init(shared->root(), masses_config), false);

	AbortIfNot_2(_cycle.register_event(orbital),
		false);

	return true;
}

/**
 * Create the shared data system
 *
 * @return True on success
 */
bool Simulation::create_shared_data()
{
	Handle<DataAccountant> accountant(new DataAccountant());

	shared.reset(new SharedData(accountant));
	AbortIfNot_2(shared, false);

	return true;
}

/**
 * Run the simulation!
 *
 * @param[in] t_stop The simulation stop time, in 100Hz steps
 *
 * @return True on success
 */
bool Simulation::go(int64 t_stop)
{
	AbortIfNot_2(_cycle.run(t_stop), false);
	return true;
}

/**
 * Initialize the simulation
 *
 * @param[in] cmd The command line
 *
 * @return True on success
 */
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

	AbortIfNot_2(cmd.get<std::string>( "telem_config", config),
		false);

	/*
	 * Create the telemetry event last to ensure it runs
	 * at the bottom of each cycle
	 */
	AbortIfNot_2(_init_telem(config), false);

	_is_init = true;
	return true;
}

/**
 * Initialize the telemetry component
 *
 * @param[in] config The telemetry config file
 *
 * @return True on success
 */
bool Simulation::_init_telem(const std::string& config)
{
	Handle<Telemetry> telemetry(new Telemetry());
	AbortIfNot_2(telemetry , false);

	AbortIfNot_2(telemetry->init(shared, config), false);

	AbortIfNot_2(_cycle.register_event(telemetry),
		false);

	return true;
}

/**
 * Initialize the simulation time keeper
 *
 * @return True on success
 */
bool Simulation::_init_time()
{
	Handle<TimeKeeper> keeper(new TimeKeeper());
	AbortIfNot_2(keeper, false);

	AbortIfNot_2(keeper->init(shared->root()), false);

	AbortIfNot_2(_cycle.register_event(keeper),
		false);

	return true;
}
