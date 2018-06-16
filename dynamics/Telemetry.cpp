#include <ctime>

#include "str_util.h"
#include "Telemetry.h"
#include "traits.h"

/**
 * @todo For traits classes, use standard types. The types used
 *       here conform to https://msdn.microsoft.com/en-us/library/s3f49ktz.aspx
 */

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

	AbortIfNot_2(_read_config(shared, config),
		false);

	std::time_t rawtime;
	struct tm* timeinfo;

	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);

	AbortIfNot_2(timeinfo, false);

	std::string now(std::asctime(timeinfo));

	std::vector<std::string> tokens;
	Util::split(now, tokens);

	AbortIf_2(tokens.size() != 5, false);

	std::string prefix =
		Util::strrep(tokens[3], ':', '.');

	for (size_t i = 0; i < _flows.size(); i++)
	{
		auto& flow = _flows[i];

		if (flow.params.size() == 0)
			continue;

		AbortIf_2(flow.file, false);

		std::string freq;
		AbortIfNot_2( Util::to_string(flow.freq, freq),
			false);

		std::string name =
			prefix + "_" + freq + "Hz.telem";

		flow.file.reset(new std::ofstream(name.c_str(),
			std::ios::out | std::ios::binary));

		AbortIfNot_2(flow.file, false);

		AbortIfNot_2(flow.file->is_open(),
			false);

		for (size_t i = 0; i < flow.params.size(); i++)
		{
			flow.params[i]->stream = flow.file;
		}
	}

	return true;
}

int64 Telemetry::dispatch(int64 t_now)
{
	for (auto& flow : _flows)
	{
		for (size_t i = 0; i < flow.params.size(); i++)
		{
			flow.params[i]->update();
		}
	}

	return 0;
}

auto Telemetry::_create_element(Handle<SharedData> shared,
								const std::string& path)
	-> Handle<stream_element>
{
	Handle<stream_element> element;

	int id  = shared->lookup(path);
	AbortIf(id < 0, false, "failed to look up '%s'",
		path.c_str());

	const std::string type = shared->get_type( id );
	AbortIf_2(type.empty(), element);

	if (type == "bool")
	{
		element.reset(new parameter< bool >(shared, id));
	}
	else if (type == "char")
	{
		element.reset(new parameter< char >(shared, id));
	}
	else if (type == "int16")
	{
		element.reset(new parameter<short >(shared, id));
	}
	else if (type == "int32")
	{
		element.reset(new parameter< int  >(shared, id));
	}
	else if (type == "int64")
	{
		element.reset(
				new parameter< long long  >(shared, id));
	}
	else if (type == "uchar")
	{
		element.reset(
			new parameter< unsigned char  >(shared, id));
	}
	else if (type == "uint16")
	{
		element.reset(
			new parameter< unsigned short >(shared, id));
	}
	else if (type == "uint32")
	{
		element.reset(
			new parameter<  unsigned int  >(shared, id));
	}
	else if (type == "uint64")
	{
		element.reset( new parameter< unsigned long long
			>(shared, id));
	}
	else if (type == "float")
	{
		element.reset(new parameter<float >(shared, id));
	}
	else if (type == "double")
	{
		element.reset(new parameter<double>(shared, id));
	}
	else
	{
		Abort(element, "invalid type: '%s'",
			type.c_str());
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

		AbortIf_2(tokens.size() != 2, false);

		size_t freq = 0;
		AbortIfNot_2(Util::from_string(tokens[1], freq),
			false);

		AbortIfNot_2(0 < freq && freq <= max_freq,
			false);

		auto& flow = _flows[freq];
		flow.freq  = freq;
		
		auto element = _create_element(shared, tokens[0]);
		AbortIfNot_2(element, false);

		flow.params.push_back(element);
	}

	return true;
}
