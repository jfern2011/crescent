#pragma once

#include "EphemerisObject.h"
#include "Event.h"
#include "SharedData.h"
#include "RK4.h"

class EphemerisManager : public Event
{
	struct SharedIDs
	{
		SharedIDs() : SharedIDs("")
		{
		}

		SharedIDs(const std::string& _name)
			: object_id(-1),
			  a_eci_id(3,-1),
			  r_eci_id(3,-1),
			  v_eci_id(3,-1),
			  mass_id(-1),
			  name(_name)
		{
		}

		int object_id;

		std::vector<int> a_eci_id;
		std::vector<int> r_eci_id;
		std::vector<int> v_eci_id;

		int mass_id;

		std::string name;

		Handle<DataDirectory>
			telemetry;
	};

public:

	/**
	 * Gravitational constant, m^3/kg/s^2
	 */
	const double G = 6.67408e-11;

	const static int64 period = 2; // 50Hz

	EphemerisManager();

	~EphemerisManager();

	void compute_accel();

	int64 dispatch(int64 t_now);

	bool init(Handle<DataDirectory> shared,
			  const std::string& config);

	void propagate();

	Vector<6> dxdt_func(double time, const Vector<6>& x0)
	{
		auto& obj =
			_subdir->load<EphemerisObject>(_ids[_dxdt_i].object_id);

		Vector<3> v_eci= obj.rv_eci.sub<3>(3);

		_dxdt_i = (_dxdt_i + 1) % _ids.size();

		return v_eci.vcat(obj.accel);
	}

private:

	bool _init_telemetry();

	bool _update_telemetry();

	size_t _dxdt_i;

	std::vector< SharedIDs >
		_ids;

	bool _is_init;

	RK4<6> _rk4;

	Handle<DataDirectory>
		_subdir;
};
