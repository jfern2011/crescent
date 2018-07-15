#pragma once

#include "EphemerisObject.h"
#include "Event.h"
#include "SharedData.h"
#include "RK4.h"

namespace Crescent
{
	/**
	 * Manages the ephemerides of all bodies within the system, which
	 * are propagated at 50Hz
	 */
	class EphemerisManager : public Event
	{
		/**
		 * Collection of IDs used to look up data for a specific body
		 */
		struct SharedIDs
		{
			/**
			 * Default constructor
			 */
			SharedIDs() : SharedIDs("")
			{
			}

			/**
			* Constructor
			*
			* @param[in] _name The name of this body corresponding to
			*                  these IDs
			*/
			SharedIDs(const std::string& _name)
				: object_id(-1),
				a_eci_id(3, -1),
				r_eci_id(3, -1),
				v_eci_id(3, -1),
				mass_id(-1),
				name(_name)
			{
			}

			/**
			 * Shared ID of the EphemerisObject which stores data for
			 * internal computations
			 */
			int object_id;

			/**
			 * Shared IDs of the three telemetry variables holding
			 * the object's acceleration coordinates
			 */
			std::vector<int> a_eci_id;

			/**
			 * Shared IDs of the three telemetry variables holding
			 * the object's position coordinates
			 */
			std::vector<int> r_eci_id;

			/**
			 * Shared IDs of the three telemetry variables holding
			 * the object's velocity coordinates
			 */
			std::vector<int> v_eci_id;

			/**
			 * Shared ID of the telemetry variable holding the mass of
			 * the object
			 */
			int mass_id;

			/**
			 * The name of the EphemerisObject associated with these
			 * shared IDs
			 */
			std::string name;

			/**
			 * The directory in which to store telemetry
			 */
			Handle<DataDirectory>
				telemetry;
		};

	public:

		/**
		 * Gravitational constant, m^3/kg/s^2
		 */
		const double G = 6.67408e-11;

		/**
		 * The dispatch rate of this Event
		 */
		const static int64 period = 2; // 50Hz

		EphemerisManager();

		~EphemerisManager();

		void compute_accel();

		int64 dispatch(int64 t_now);

		bool init(Handle<DataDirectory> shared,
			const std::string& config);

		void propagate();

	private:

		bool _init_telemetry();

		bool _update_telemetry();

		/**
		 * dx/dt of the current EphemerisObject (unused)
		 */
		size_t _dxdt_i;

		/**
		 * The SharedIDs of each body
		 */
		std::vector< SharedIDs >
			_ids;

		/**
		 * True if initialized
		 */
		bool _is_init;

		/**
		 * The RK4 propagator (unused)
		 */
		RK4<6> _rk4;

		/**
		 * The directory in which to store our
		 * internal computations
		 */
		Handle<DataDirectory>
			_subdir;
	};
}
