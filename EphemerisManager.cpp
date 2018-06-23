#include "EphemerisManager.h"
#include "dynamics/Verbosity.h"

EphemerisManager::EphemerisManager()
	: Event("Ephemeris"),
	  _dxdt_i(0),
	  _ids(),
	  _is_init(false),
	  _rk4(0.02),
	  _subdir()
{
}

EphemerisManager::~EphemerisManager()
{
}

/**
 * Compute the accelerations of all objects in the system. The
 * governing equation is 1.2-10 in reference (1)
 */
void EphemerisManager::compute_accel()
{
	for (size_t i = 0; i < _ids.size(); i++)
	{
		auto& m_i =
			_subdir->load<EphemerisObject>(_ids[i].object_id);

		m_i.accel.zeroify();

		if (Verbosity::is_debug())
		{
			std::printf("computing accel for '%s':\n",
				m_i.name.c_str());
		}

		for (size_t j = 0; j < _ids.size(); j++)
		{
			if (i == j) continue;

			auto& m_j = 
				_subdir->load<EphemerisObject>(_ids[j].object_id);

			if (Verbosity::is_debug())
			{
				std::printf("perturbing body: '%s'\n",
					m_j.name.c_str());
			}

			Vector<3> r_ji =
				m_i.rv_eci.sub<3>(0) - m_j.rv_eci.sub<3>(0);

			if (Verbosity::is_debug())
			{
				std::printf("position from '%s' to '%s':\n",
					m_j.name.c_str(), m_i.name.c_str());
				r_ji.print();
			}

			const double norm = r_ji.norm();

			if (norm == 0.0) continue;

			m_i.accel -=
				G * m_j.mass / Util::pow(norm, 3) * r_ji;

			if (Verbosity::is_debug())
			{
				std::printf("accel[%s] = \n",
					m_i.name.c_str());
				m_i.accel.print();
			}
		}
	}
}

int64 EphemerisManager::dispatch(int64 t_now)
{
	if (t_now % period) return 0;

	/*
	 * 1. Compute the accelerations of all objects
	 */
	compute_accel();

	/*
	 * 2. Propagate forward by 1 step
	 */
	propagate();

	/*
	 * 3. Update telemetry output values
	 */
	AbortIfNot_2(_update_telemetry(),
		false);

	return 0;
}

bool EphemerisManager::init(Handle<DataDirectory> shared,
							const std::string & config)
{
	AbortIf_2(_is_init, false);

	std::vector<std::string> lines;
	AbortIfNot_2(Util::read_config(config, lines), false);

	_subdir = shared->subdir("orbital");
	AbortIfNot_2(_subdir, false);

	for (auto& line : lines)
	{
		std::vector<std::string> tokens;
		Util::split(line, tokens);

		AbortIf(tokens.size() != 7, false,
			"incomplete ephemeris for '%s'" , tokens[0].c_str());

		const std::string path = tokens[0];

		auto dir = _subdir->lookup(path);
		AbortIfNot(dir, false, "cannot find '%s'", path.c_str());

		SharedIDs ids(path);
		ids.object_id = dir->get_element_id("internal");

		auto object =
			dir->get_element<EphemerisObject>(ids.object_id);

		AbortIfNot_2(object, false);

		_ids.push_back(ids);

		for (int i = 0; i < 6; i++)
		{
			AbortIfNot_2(Util::from_string<double>(tokens[i+1],
				object->get().rv_eci(i)), false);
		}
	}

	AbortIfNot_2(_init_telemetry(), false);

	_is_init = true;
	return true;
}

void EphemerisManager::propagate()
{
	for (auto iter = _ids.begin(), end = _ids.end();
		iter != end; ++iter)
	{
		auto& obj = _subdir->load<EphemerisObject>(iter->object_id);

		Vector<3> v_eci = obj.rv_eci.sub<3>(3);

		Vector<6> dxdt = v_eci.vcat(obj.accel);

		double dt = 1.0 / 100 * period;

		obj.rv_eci += dxdt * dt;
	}
}

bool EphemerisManager::_init_telemetry()
{
	for (auto iter = _ids.begin(), end = _ids.end();
		 iter != end; ++iter)
	{
		iter->telemetry = _subdir->subdir(iter->name)->subdir("telemetry");
		AbortIfNot_2(iter->telemetry, false);

		int id = iter->telemetry->create_element<double>("a_eci.0");
		iter->a_eci_id[0] = id;

			id = iter->telemetry->create_element<double>("a_eci.1");
		iter->a_eci_id[1] = id;

			id = iter->telemetry->create_element<double>("a_eci.2");
		iter->a_eci_id[2] = id;
		
			id = iter->telemetry->create_element<double>("r_eci.0");
		iter->r_eci_id[0] = id;

			id = iter->telemetry->create_element<double>("r_eci.1");
		iter->r_eci_id[1] = id;

			id = iter->telemetry->create_element<double>("r_eci.2");
		iter->r_eci_id[2] = id;

			id = iter->telemetry->create_element<double>("v_eci.0");
		iter->v_eci_id[0] = id;

			id = iter->telemetry->create_element<double>("v_eci.1");
		iter->v_eci_id[1] = id;

			id = iter->telemetry->create_element<double>("v_eci.2");
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
		auto& object = _subdir->load<EphemerisObject>(iter->object_id);

		for (int i = 0; i <= 2; i++)
		{
			iter->telemetry->load<double>(iter->a_eci_id[i])
				= object.accel(i);

			iter->telemetry->load<double>(iter->r_eci_id[i])
				= object.rv_eci(i);

			iter->telemetry->load<double>(iter->v_eci_id[i])
				= object.rv_eci(i+3);
		}
	}

	return true;
}
