#include "Orbital.h"

/**
 * Constructor
 */
Orbital::Orbital()
	: Event("Orbital"),
	  _data(), _ids(), _is_init(false), _name2mass()
{
}

/**
 * Destructor
 */
Orbital::~Orbital()
{
}

/**
 * Initialize.
 *
 * @param[in] shared The directory under which to store this
 *                   component's data
 * @param[in] config The masses config file
 *
 * @return True on success
 */
bool Orbital::init(Handle<DataDirectory> shared,
				   const std::string& masses_config)
{
	AbortIf_2(_is_init || !shared, false);

	_data = shared->subdir("orbital");

	AbortIfNot_2(_read_masses_config(masses_config), false);

	AbortIfNot_2(_init_shared(), false);

	_is_init = true;
	return true;
}

/**
 * Run this component
 *
 * @todo Implement mass loss
 *
 * @param[in] t_now The current simulation time
 *
 * @return 0 on success
 */
int64 Orbital::dispatch(int64 t_now)
{
	AbortIfNot_2(_is_init, -1);
	return 0;
}

/**
 * Check if an EphemerisObject with the given name exists
 *
 * @param[in] name The name of the object
 *
 * @return True if it exists
 */
bool Orbital::exists(const std::string& name) const
{
	for (auto iter = _name2mass.begin(), end = _name2mass.end();
		 iter != end; ++iter)
	{
		if (Util::trim(name) == iter->first) return true;
	}

	return false;
}

/**
 * Create the shared data structures used by downstream
 * algorithms
 *
 * @return True on success
 */
bool Orbital::_init_shared()
{
	AbortIfNot_2(_data, false);

	for (auto iter  = _name2mass.begin(), end  = _name2mass.end();
		 iter != end; ++iter)
	{
		SharedIDs ids;

		auto dir = _data->subdir(iter->first);
		AbortIfNot_2(dir, false);

		int id = dir->create_element<EphemerisObject>("internal");
		AbortIf_2(id < 0, false);

		ids.object_id = id;

		auto& object = dir->load<EphemerisObject>(id);
		object.mass = iter->second;
		object.name = iter->first;

		dir = dir->subdir("telemetry");
		AbortIfNot_2(dir, false);

		id = dir->create_element<double>("mass");
		AbortIf_2(id < 0, false);

		ids.mass_id = id;

		_ids.push_back(ids);

		dir->load<double>(id) =
			iter->second;
	}

	return true;
}

/**
 * Read the masses config file
 *
 * @param[in] name The name of the file to parse
 *
 * @return True on success
 */
bool Orbital::_read_masses_config(const std::string& name)
{
	std::vector<std::string> lines;
	AbortIfNot_2(Util::read_config(name, lines), false);

	for (auto& line : lines)
	{
		std::vector<std::string> tokens;
		Util::split(line, tokens);

		double mass = 0.0;

		if (tokens.size() > 1)
		{
			AbortIfNot_2(Util::from_string(tokens[1], mass),
				false);
		}

		if (tokens.size() > 0)
		{
			AbortIf_2(exists(tokens[0]),
				false);

			_name2mass[Util::trim(tokens[0])]
				= mass;
		}
	}

	return true;
}
