#include "str_util.h"
#include "Telemetry.h"
#include "traits.h"

Telemetry::Telemetry()
	: Event("Telemetry"), _flows(max_freq + 1)
{
}

Telemetry::~Telemetry()
{
}

bool Telemetry::init(Handle<SharedData> shared,
	const std::string& config)
{
	AbortIfNot_2(shared, false);
	return true;
}

int64 Telemetry::dispatch(int64 t_now)
{
	return 0;
}

auto Telemetry::_create_element(Handle<SharedData> shared,
								const std::string& path,
								const std::string& type)
	-> Handle<stream_element>
{
	Handle<stream_element> element;

	int id = shared->lookup(path);
	AbortIf_2(id < 0, false);

	if (type == "bool")
	{
		element.reset(new parameter<bool>(shared, id));
	}
	else if (type == "char")
	{
		element.reset(new parameter<bool>(shared, id));
	}
	
	return element;
}

bool Telemetry::_read_config(Handle<SharedData> shared,
							 const std::string& name)
{
	std::vector<std::string> lines;
	AbortIfNot_2(Util::read_config(name, lines), false);

	for (auto& line : lines)
	{
		std::vector<std::string> tokens;
		Util::split(line, tokens);

		AbortIf_2(tokens.size() != 3, false);

		size_t freq = 0;
		AbortIfNot_2(Util::from_string(tokens[2], freq),
			false);

		AbortIfNot_2(0 < freq && freq < max_freq,
			false);

		if (type == "double")
		{

		}



		double mass = 0.0;

		if (tokens.size() > 1)
		{
			AbortIfNot_2(Util::from_string(tokens[1], mass),
				false);
		}

		if (tokens.size() > 0)
		{
			AbortIf_2(exists(tokens[0]), false);

			_name2mass[Util::trim(tokens[0])]
				= mass;
		}
	}

	return true;
}