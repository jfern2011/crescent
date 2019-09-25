#include <chrono>

#include "service_module_rcs_quad.h"

#include "abort.h"

namespace Crescent
{
	ServiceModuleRcsQuad::ServiceModuleRcsQuad(std::int64_t dt)
		: m_hePress(),
		  m_priOxTank(),
		  m_secOxTank(),
		  m_priFuelTank(),
		  m_secFuelTank(),
		  m_priOxIsolationValve(),
		  m_secOxIsolationValve(),
		  m_priFuelIsolationValve(),
		  m_secFuelIsolationValve(),
		  m_dt(dt),
		  m_id2index(),
		  m_thrusters()
	{
		m_id2index[ThrusterId::a1] = 0;
		m_id2index[ThrusterId::a2] = 1;
		m_id2index[ThrusterId::a3] = 2;
		m_id2index[ThrusterId::a4] = 3;
	}

	bool ServiceModuleRcsQuad::isFiring(
		ThrusterId thruster) const
	{
		TankId fuelId, oxId;
		AbortIfNot_2(fuelReady(fuelId), false);
		AbortIfNot_2(oxReady  (  oxId), false);

		const bool fuelOk = fuelId != TankId::invalid;
		const bool oxOk   = oxId   != TankId::invalid;

		auto iter = m_id2index.find(thruster);

		return fuelOk && oxOk &&
			m_thrusters[iter->second]->isFiring();
	}

	bool ServiceModuleRcsQuad::update()
	{
		for (auto thruster : { ThrusterId::a1,
							   ThrusterId::a2,
							   ThrusterId::a3,
							   ThrusterId::a4 })
		{
			if (isFiring(thruster))
				depleteTanks();
		}

		return true;
	}

	bool ServiceModuleRcsQuad::depleteTanks()
	{
		AbortIfNot_2(m_priFuelTank, false);
		AbortIfNot_2(m_priOxTank, false);
		AbortIfNot_2(m_secFuelTank, false);
		AbortIfNot_2(m_secOxTank, false);

		TankId fuelId, oxId;
		AbortIfNot_2(fuelReady(fuelId), false);
		AbortIfNot_2(  oxReady(  oxId), false);

		if (fuelId == TankId::invalid ||
			oxId   == TankId::invalid)
			return true;

		AbortIfNot_2( m_dt > 0, false );

		std::chrono::nanoseconds dtNano(m_dt);
		const double dtSec =
			std::chrono::duration<double>(dtNano).count();

		if (fuelId == TankId::primary)
			m_priFuelTank->m_mass
				-= (fuelFlowRate * dtSec);
		else
			m_secFuelTank->m_mass
				-= (fuelFlowRate * dtSec);

		if (oxId   == TankId::primary)
			m_priOxTank->m_mass
				-= (oxFlowRate   * dtSec);
		else
			m_secOxTank->m_mass
				-= (oxFlowRate   * dtSec);

		return true;
	}

	bool ServiceModuleRcsQuad::fuelReady(TankId& tank) const
	{
		AbortIfNot_2(m_priFuelIsolationValve, false);
		AbortIfNot_2(m_priFuelTank, false);
		AbortIfNot_2(m_secFuelIsolationValve, false);
		AbortIfNot_2(m_secFuelTank, false);

		tank = TankId::invalid;
		
		const bool primaryReady =
			m_priFuelTank->isPressurized() &&
			!m_priFuelTank->isDepleted() &&
			m_priFuelIsolationValve->isOpen();

		const bool secondaryReady = 
			m_secFuelTank->isPressurized() &&
			!m_secFuelTank->isDepleted() &&
			m_secFuelIsolationValve->isOpen();

		AbortIf_2(primaryReady && secondaryReady,
			false);

		if (primaryReady)
			tank = TankId::primary;
		else if (secondaryReady)
			tank = TankId::secondary;

		return true;
	}

	bool ServiceModuleRcsQuad::oxReady(TankId& tank) const
	{
		AbortIfNot_2(m_priOxIsolationValve, false);
		AbortIfNot_2(m_priOxTank, false);
		AbortIfNot_2(m_secOxIsolationValve, false);
		AbortIfNot_2(m_secOxTank, false);

		tank = TankId::invalid;

		const bool primaryReady =
			m_priOxTank->isPressurized() &&
			!m_priOxTank->isDepleted() &&
			m_priOxIsolationValve->isOpen();

		const bool secondaryReady =
			m_secOxTank->isPressurized() &&
			!m_secOxTank->isDepleted() &&
			m_secOxIsolationValve->isOpen();

		AbortIf_2(primaryReady && secondaryReady,
			false);

		if (primaryReady)
			tank = TankId::primary;
		else if (secondaryReady)
			tank = TankId::secondary;

		return true;
	}
}
