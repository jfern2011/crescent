#include "Orbital.h"

Orbital::Orbital() : Event("Orbital"), _bodies()
{
}

Orbital::~Orbital()
{
}

int64 Orbital::dispatch(int64 t_now)
{
	return 0;
}
