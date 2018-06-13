#pragma once

#include "Event.h"
#include <fstream>
#include "SharedData.h"

class Telemetry : public Event
{
	struct parameter
	{
		parameter(const std::string& _path, const std::string& _type)
			: path(_path), type(_type)
		{
		}

		std::string path;
		std::string type;
	};

	struct flow
	{
		std::ofstream file;

		int64 freq;

		std::vector<parameter>
			params;
	};

public:

	Telemetry();

	~Telemetry();

	bool init(Handle<SharedData> shared, // timestamp_xHz.telem
		const std::string& config);

	int64 dispatch(int64 t_now);

private:

	bool _read_config(const std::string& name);

	std::vector<flow> _flows;
};
