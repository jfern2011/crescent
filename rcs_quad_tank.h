#pragma once

#include <memory>

#include "tank.h"
#include "valve.h"

namespace Crescent
{
	class RcsQuadTank final : public Tank
	{

	public:

		RcsQuadTank();

		RcsQuadTank(const RcsQuadTank& tank)            = default;
		RcsQuadTank(RcsQuadTank&& tank)                 = default;
		RcsQuadTank& operator=(const RcsQuadTank& tank) = default;
		RcsQuadTank& operator=(RcsQuadTank&& tank)      = default;

		~RcsQuadTank() = default;

		bool install(std::shared_ptr<ValvePair> checkValves);

		bool isDepleted()    const override;
		bool isPressurized() const override;

	private:
		
		std::shared_ptr<ValvePair>
			m_checkValves;
	};
}
