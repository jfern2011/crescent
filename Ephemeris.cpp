#include "Ephemeris.h"

EphemerisManager::EphemerisManager()
	: Event("Ephemeris"),
	  _ids(),
	  _is_init(false),
	  _subdir(),
	  _telemetry()
{
}

EphemerisManager::~EphemerisManager()
{
}

int64 EphemerisManager::dispatch(int64 t_now)
{
	for (size_t i = 0; i < _ids.size(); i++)
	{

	}

	return 0;
}

bool EphemerisManager::init(Handle<DataDirectory> shared,
							const std::string & config)
{
	AbortIf_2(_is_init, false);

	std::vector<std::string> lines;
	AbortIfNot_2(Util::read_config(config, lines), false);

	_subdir = shared;

	for (auto& line : lines)
	{
		std::vector<std::string> tokens;
		Util::split(line, tokens);

		AbortIf(tokens.size() != 7, false,
			"incomplete ephemeris for '%s'" , tokens[0].c_str());

		const std::string path = tokens[0];

		auto dir = shared->lookup(path);
		AbortIfNot(dir, false, "cannot find '%s'", path.c_str());

		auto object =
			dir->get_element<EphemerisObject>("object");

		AbortIfNot_2(object, false);

		for (int i = 0; i <= 2; i++)
		{
			AbortIfNot_2(Util::from_string<double>(tokens[i+1],
				object->get().r_eci(i)), false);

			AbortIfNot_2(Util::from_string<double>(tokens[i+4],
				object->get().v_eci(i)), false);
		}
	}

	AbortIfNot_2(_init_telemetry(), false);

	_is_init = true;
	return true;
}

bool EphemerisManager::_init_telemetry()
{
	_telemetry = _subdir->subdir("telemetry");

	AbortIfNot_2(_telemetry, false);

	for (auto iter = _ids.begin(), end = _ids.end();
		 iter != end; ++iter)
	{
		int id = _telemetry->create_element<double>("a_eci.0");
		iter->a_eci_id[0] = id;

			id = _telemetry->create_element<double>("a_eci.1");
		iter->a_eci_id[1] = id;

			id = _telemetry->create_element<double>("a_eci.2");
		iter->a_eci_id[2] = id;
		
			id = _telemetry->create_element<double>("r_eci.0");
		iter->r_eci_id[0] = id;

			id = _telemetry->create_element<double>("r_eci.1");
		iter->r_eci_id[1] = id;

			id = _telemetry->create_element<double>("r_eci.2");
		iter->r_eci_id[2] = id;

			id = _telemetry->create_element<double>("v_eci.0");
		iter->v_eci_id[0] = id;

			id = _telemetry->create_element<double>("v_eci.1");
		iter->v_eci_id[1] = id;

			id = _telemetry->create_element<double>("v_eci.2");
		iter->v_eci_id[2] = id;

		for (int i = 0; i <= 2; i++)
		{
			AbortIf_2(iter->a_eci_id[i] < 0, false);
			AbortIf_2(iter->r_eci_id[i] < 0, false);
			AbortIf_2(iter->v_eci_id[i] < 0, false);
		}
	}

	AbortIfNot_2(_update_telemetry(), false);

	return true;
}

bool EphemerisManager::_update_telemetry()
{
	for (auto iter = _ids.begin(), end = _ids.end();
		 iter != end; ++iter)
	{
		auto& object =
			_subdir->load<EphemerisObject>(iter->object_id);

		for (int i = 0; i <= 2; i++)
		{
			_telemetry->load<double>(iter->a_eci_id[i])
				= object.accel(i);

			_telemetry->load<double>(iter->r_eci_id[i])
				= object.accel(i);

			_telemetry->load<double>(iter->v_eci_id[i])
				= object.accel(i);
		}
	}

	return true;
}
