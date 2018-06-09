#include "Telemetry.h"

Telemetry::Telemetry()
	: Event("Telemetry")
{
}

Telemetry::~Telemetry()
{
}

bool Telemetry::init(Handle<SharedData> shared,
	const std::string& config)
{
	return true;
}

int64 Telemetry::dispatch(int64 t_now)
{
	return 0;
}
