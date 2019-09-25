#pragma once

namespace Crescent
{
	class Tank
	{

	public:

		Tank();

		Tank(const Tank& tank)            = default;
		Tank(Tank&& tank)                 = default;
		Tank& operator=(const Tank& tank) = default;
		Tank& operator=(Tank&& tank)      = default;

		~Tank() = default;

		virtual bool isDepleted()    const = 0;
		virtual bool isPressurized() const = 0;

		double m_mass;

	private:

	};
}
