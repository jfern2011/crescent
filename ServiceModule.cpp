#include "ServiceModule.h"

namespace Crescent
{
	/**
	 * Constructor
	 *
	 * @param[in] detached If true, we're initially detached from the
	 *                     command module
	 */
	ServiceModule::ServiceModule(bool detached)
		: _is_attached(!detached)
	{
	}

	/**
	 * Destructor
	 */
	ServiceModule::~ServiceModule()
	{
	}

	/**
	 * Detach from the command module
	 */
	void ServiceModule::detach()
	{
		_is_attached = false;
	}

	/**
	 * Check if we're detached from the command module
	 *
	 * @return True if detached
	 */
	bool ServiceModule::is_detached() const
	{
		return !_is_attached;
	}
}
