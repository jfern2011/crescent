#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>

#include "service_module_rcs_press.h"
#include "service_module_rcs_thruster.h"
#include "tank.h"

namespace Crescent
{
	/**
	 * Based upon https://history.nasa.gov/afj/aoh/aoh-v1-2-05-rcs.pdf
	 * Page 2.5-9/2.5-10
	 *
	 * https://www.hq.nasa.gov/alsj/LM10_Reaction_Control_ppRC1-12.pdf
	 */
	class ServiceModuleRcsQuad final
	{
		enum class TankId
		{
			primary,
			secondary,
			invalid
		};

	public:

		const double fuelFlowRate = 0.053070307; // kg/s
		const double oxFlowRate   = 0.108862200; // kg/s

		enum class ThrusterId
		{
			a1,
			a2,
			a3,
			a4
		};

		ServiceModuleRcsQuad(std::int64_t dt);

		ServiceModuleRcsQuad(const ServiceModuleRcsQuad& quad)            = default;
		ServiceModuleRcsQuad(ServiceModuleRcsQuad&& quad)                 = default;
		ServiceModuleRcsQuad& operator=(const ServiceModuleRcsQuad& quad) = default;
		ServiceModuleRcsQuad& operator=(ServiceModuleRcsQuad&& quad)      = default;

		~ServiceModuleRcsQuad() = default;

		bool isFiring(ThrusterId thruster) const;

		bool update();

		std::shared_ptr< ServiceModuleRcsPress >
			m_hePress;

		std::shared_ptr<Tank> m_priOxTank;
		std::shared_ptr<Tank> m_secOxTank;

		std::shared_ptr<Tank> m_priFuelTank;
		std::shared_ptr<Tank> m_secFuelTank;

		std::shared_ptr<Valve> m_priOxIsolationValve;
		std::shared_ptr<Valve> m_secOxIsolationValve;

		std::shared_ptr<Valve>
			m_priFuelIsolationValve;
		std::shared_ptr<Valve>
			m_secFuelIsolationValve;

	private:

		bool depleteTanks();

		bool fuelReady(TankId& tank) const;
		bool oxReady  (TankId& tank) const;

		std::int64_t m_dt;

		std::map<ThrusterId, size_t>
			m_id2index;

		std::array<std::shared_ptr<
			ServiceModuleRcsThruster>, 4>
				m_thrusters;
	};
}
