#include "rcs_quad_tank.h"

#include "abort.h"

namespace Crescent
{
	RcsQuadTank::RcsQuadTank() : m_checkValves()
	{
	}

	bool RcsQuadTank::install(
		std::shared_ptr<ValvePair> checkValves)
	{
		AbortIfNot_2(checkValves, false);

		m_checkValves = checkValves;
		return true;
	}

	bool RcsQuadTank::isDepleted() const
	{
		return m_mass <= 0.0;
	}

	bool RcsQuadTank::isPressurized() const
	{
		AbortIfNot_2(m_checkValves, false);

		return (m_checkValves->first .isOpen() ||
				m_checkValves->second.isOpen());
	}
}
