#pragma once

#include <memory>

#include "valve.h"

namespace Crescent
{
	/**
	 * @todo Fuel valve opens ~2 ms prior to Ox valve
	 */
	class ServiceModuleRcsThruster final
	{

	public:

		ServiceModuleRcsThruster();

		ServiceModuleRcsThruster(const ServiceModuleRcsThruster& thruster)            = default;
		ServiceModuleRcsThruster(ServiceModuleRcsThruster&& thruster)                 = default;
		ServiceModuleRcsThruster& operator=(const ServiceModuleRcsThruster& thruster) = default;
		ServiceModuleRcsThruster& operator=(ServiceModuleRcsThruster&& thruster)      = default;

		~ServiceModuleRcsThruster() = default;

		bool init(std::shared_ptr<ValvePair> valves);

		bool isFiring() const;

	private:

		/**
		 * Oxidizer and fuel valves
		 */
		std::shared_ptr< ValvePair >
			m_valves;
	};
}
