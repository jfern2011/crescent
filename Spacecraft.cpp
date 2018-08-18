#include "abort.h"
#include "Spacecraft.h"

namespace Crescent
{
	/**
	 * Constructor
	 */
	Spacecraft::Spacecraft() : cm(), lm(), sm()
	{
	}

	/**
	 * Destructor
	 */
	Spacecraft::~Spacecraft()
	{
	}

	/**
	 * Detach the service module from the command module
	 */
	void Spacecraft::detach_sm()
	{
		cm.detach_sm(); sm.detach();
	}

	/**
	 * Attempt to dock the command module with the lunar
	 * module
	 *
	 * @return True on success
	 */
	bool Spacecraft::dock(bool val)
	{
		const bool cm_ok = cm.dock(val);
		const bool lm_ok = lm.dock(val);

		AbortIfNot_2(cm_ok == lm_ok,
			false);

		return cm_ok;
	}
}
