#include "Orbital.h"

Orbital::Orbital()
	: Event("Orbital"), _bodies(), _data(), _is_init(false)
{
}

Orbital::~Orbital()
{
}

bool Orbital::init(Handle<DataDirectory> shared,
				   const std::string& masses_config,
				   const std::string& ephem_config)
{
	AbortIfNot_2(_read_masses_config(masses_config), false);

	AbortIfNot_2(_read_ephem_config( ephem_config ), false);

	AbortIfNot_2(_init_shared(shared), false);

	_is_init = true;
	return true;
}

int64 Orbital::dispatch(int64 t_now)
{
	return 0;
}

int Orbital::exists(const std::string& name) const
{
	for (size_t i = 0; i < _bodies.size(); i++)
	{
		if ( _bodies[i].name == Util::trim(name) )
			return i;
	}

	return -1;
}

bool Orbital::_init_shared(Handle<DataDirectory> shared)
{
	AbortIfNot_2(shared, false);

	this->_data = shared->subdir("orbital");
	AbortIfNot_2(_data, false);

	auto rb_dir = _data->subdir("bodies");
	AbortIfNot_2(rb_dir, false);

	for (auto iter = _bodies.begin(), end = _bodies.end();
		 iter != end; ++iter)
	{
		auto dir = rb_dir->subdir(iter->name);
		AbortIfNot_2(dir, false);

		for (int i = 0; i <= 2; i++)
		{
			std::string ind;
			AbortIfNot_2(Util::to_string(i, ind), false);

			iter->accel_id[i]
				= dir->create_element<double>("accel." + ind);

			AbortIf_2(iter->accel_id[i] < 0,
				false);
		}

		iter->mass_id = dir->create_element<double>("mass");
		AbortIf_2(iter->mass_id < 0, false);

		iter->name_id
			= dir->create_element<std::string>("name");
		AbortIf_2(iter->name_id < 0, false);

		for (int i = 0; i <= 5; i++)
		{
			std::string ind;
			AbortIfNot_2(Util::to_string(i, ind), false);

			iter->state_id[i]
				= dir->create_element<double>("state." + ind);

			AbortIf_2(iter->state_id[i] < 0,
				false);
		}
	}

	return true;
}

bool Orbital::_read_ephem_config(const std::string& name)
{
	std::vector<std::string> lines;
	AbortIfNot_2(Util::read_config(name, lines), false);

	for (auto& line : lines)
	{
		// todo
	}

	return true;
}

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
			AbortIfNot_2( exists(tokens[0]) < 0, false );
			_bodies.push_back(
				RigidBody(Util::trim(tokens[0]),
						  mass));
		}
	}

	return true;
}
