#include "valve.h"

namespace Crescent
{
	Valve::Valve() : m_isOpen(false)
	{
	}

	bool Valve::close(std::uint64_t)
	{
		m_isOpen = false;
		return true;
	}

	bool Valve::isOpen() const
	{
		return m_isOpen;
	}

	bool Valve::open( std::uint64_t)
	{
		m_isOpen = true;
		return true;
	}
}
