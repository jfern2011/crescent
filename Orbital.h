#pragma once

#include <map>
#include <vector>

#include "EphemerisObject.h"
#include "Event.h"
#include "SharedData.h"

/**
 * An event which keeps a record of all bodies within the
 * system
 */
class Orbital : public Event
{
	/**
	 * Collection of IDs used to look up data for a specific body
	 */
	struct SharedIDs
	{
		SharedIDs() : object_id(-1), mass_id(-1)
		{
		}

		SharedIDs(int _object_id, int _mass_id)
			: object_id(_object_id), mass_id(_mass_id)
		{
		}

		/**
		 * Shared ID of the EphemerisObject which stores data for
		 * internal computations
		 */
		int object_id;

		/**
		* Shared ID of the telemetry variable holding the mass of
		* the object
		*/
		int mass_id;
	};

public:

	Orbital();

	~Orbital();

	bool init(Handle<DataDirectory> shared,
		      const std::string& masses_config);

	int64 dispatch(int64 t_now);

	bool exists(const std::string& name) const;

private:

	bool _init_shared();

	bool _read_masses_config(const std::string& name);

	/**
	 * The directory in which to create storage
	 * for internal computations
	 */
	Handle<DataDirectory> _data;

	/**
	 * The SharedIDs of each body
	 */
	std::vector<SharedIDs>
		_ids;

	/**
	 * True if initialized
	 */
	bool _is_init;

	/**
	 * Maps an object's name to its mass per the
	 * masses config file
	 */
	std::map<std::string, double>
		_name2mass;
};
