#pragma once

#include <cstdint>
#include <utility>

namespace Crescent
{
	class Valve final
	{

	public:

		Valve();

		Valve(const Valve& valve)            = default;
		Valve(Valve&& valve)                 = default;
		Valve& operator=(const Valve& valve) = default;
		Valve& operator=(Valve&& valve)      = default;

		~Valve() = default;

		bool close(std::uint64_t data);

		bool isOpen() const;

		bool open( std::uint64_t data);

	private:

		bool m_isOpen;
	};

	using ValvePair =
		std::pair<Valve, Valve>;
}
