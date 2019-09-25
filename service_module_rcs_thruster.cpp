#include "service_module_rcs_thruster.h"

#include "abort.h"

namespace Crescent
{
	ServiceModuleRcsThruster::ServiceModuleRcsThruster()
		: m_valves()
	{
	}

	bool ServiceModuleRcsThruster::init(
		std::shared_ptr< ValvePair > valves)
	{
		AbortIfNot_2(valves, false);

		m_valves = valves;
		return true;
	}

	bool ServiceModuleRcsThruster::isFiring() const
	{
		AbortIfNot_2(m_valves, false);

		const bool firing =
			m_valves->first.isOpen() && m_valves->second.isOpen();

		return firing;
	}
}
