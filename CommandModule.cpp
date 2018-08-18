#include "CommandModule.h"

namespace Crescent
{
	/**
	* Constructor
	*
	* @param[in] docked   True if we're initially docked with the
	*                     lunar module
	* @param[in] detached If true, we're initially detached from the
	*                     service module
	*/
	CommandModule::CommandModule(bool docked, bool detached)
		: _is_docked(docked), _is_attached(!detached)
	{
	}

	/**
	* Destructor
	*/
	CommandModule::~CommandModule()
	{
	}

	/**
	 * Detach from the service module
	 */
	void CommandModule::detach_sm()
	{
		_is_attached = false;
	}

	/**
	* Dock or undock with the lunar module
	*
	* @param[in] val  If true, dock; otherwise, undock from the
	*                 lunar module
	*
	* @return True on success
	*/
	bool CommandModule::dock(bool val)
	{
		_is_docked = val;
		return true;
	}

	/**
	 * Check if the service module is detached
	 *
	 * @return True if detached
	 */
	bool CommandModule::is_detached() const
	{
		return !_is_attached;
	}

	/**
	* Check if we're currently docked with the
	* lunar module
	*
	* @return True if docked
	*/
	bool CommandModule::is_docked() const
	{
		return _is_docked;
	}
}
