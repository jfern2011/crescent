#pragma once

#include <memory>

#include "valve.h"

namespace Crescent
{
	class ServiceModuleRcsPress final
	{

	public:

		ServiceModuleRcsPress();

		ServiceModuleRcsPress(const ServiceModuleRcsPress& press)            = default;
		ServiceModuleRcsPress(ServiceModuleRcsPress&& press)                 = default;
		ServiceModuleRcsPress& operator=(const ServiceModuleRcsPress& press) = default;
		ServiceModuleRcsPress& operator=(ServiceModuleRcsPress&& press)      = default;

		~ServiceModuleRcsPress() = default;

		std::shared_ptr<ValvePair>
			m_fuelCheckValves;

		std::shared_ptr< Valve >
			m_fuelReliefValve;
		
		std::shared_ptr<ValvePair>
			m_oxCheckValves;

		std::shared_ptr< Valve >
			m_oxReliefValve;

	private:
	};
}
