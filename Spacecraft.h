#pragma once

#include "CommandModule.h"
#include "LunarModule.h"
#include "ServiceModule.h"

namespace Crescent
{
	/**
	 * @class Spacecraft
	 *
	 * The Apollo spacecraft, which is composed of a \ref LunarModule,
	 * a \ref CommandModule and a \ref ServiceModule. This class treats
	 * these vehicles as a single entity during certain mission phases
	 * and as separate vehicles in others. The EphemerisManager always
	 * sees these vehicles as separate bodies, and therefore will apply
	 * the acceleration of a thrusting vehicle only to itself; this
	 * class makes sure a thrust (or torque) gets imparted to all three
	 * vehicles appropriately so that EphemerisManager may propagate
	 * each of them correctly
	 */
	class Spacecraft
	{

	public:

		Spacecraft();

		~Spacecraft();

		void detach_sm();

		bool dock(bool val);

		/** Command module */
		CommandModule cm;

		/** Lunar module   */
		LunarModule   lm;

		/** Service module */
		ServiceModule sm;

	private:

	};
}
