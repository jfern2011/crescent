#pragma once

#include "Event.h"
#include "SharedData.h"

class Telemetry : public Event
{

public:

	Telemetry();

	~Telemetry();

	bool init(Handle<SharedData> shared,
		const std::string& config);

	int64 dispatch(int64 t_now);

private:

};
