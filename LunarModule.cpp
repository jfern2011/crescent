#include "LunarModule.h"

namespace Crescent
{
	/**
	 * Constructor
	 *
	 * @param[in] docked True if we're initially docked with the
	 *                   command module
	 */
	LunarModule::LunarModule(bool docked)
		: _is_docked(docked)
	{
	}

	/**
	 * Destructor
	 */
	LunarModule::~LunarModule()
	{
	}

	/**
	 * Dock or undock with the command module
	 *
	 * @param[in] val  If true, dock; otherwise, undock from the
	 *                 command module
	 *
	 * @return True on success
	 */
	bool LunarModule::dock(bool val)
	{
		_is_docked = val;
		return true;
	}

	/**
	 * Check if we're currently docked with the
	 * command module
	 *
	 * @return True if docked
	 */
	bool LunarModule::is_docked() const
	{
		return _is_docked;
	}
}
