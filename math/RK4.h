#ifndef __RK4_H__
#define __RK4_H__

#include "Event.h"
#include "crescent.h"
#include "Vector.h"

namespace Crescent
{
	/**
	 * Implements the 4th order Runge-Kutta method
	 */
	template <size_t N>
	class RK4
	{

	public:

		/**
		 * A function F which can be registered with \ref register_dxdt
		 * where dx/dt = F(t,x)
		 */
		using Function =
			Vector<N>(*)(double time, const Vector<N>&);

		RK4(double step_size);

		~RK4();

		int64 propagate_to(int64 t_now,
			int64 dt,
			const Vector<N>& in,
			Vector<N>& out);

		bool register_dxdt(Function func);

		int64 step(int64 t_now,
			const Vector<N>& in,
			Vector<N>& out);

	private:

		/**
		 * Computes the derivative dx/dt
		 */
		Function _func;

		/**
		 * The step size (seconds)
		 */
		double _step_size;
	};

#endif

	/**
	 * Constructor
	 *
	 * @param[in] step_size The integration step size
	 */
	template <size_t N>
	RK4<N>::RK4(double step_size)
		: _func(nullptr), _step_size(step_size)
	{
	}

	/**
	 * Destructor
	 */
	template <size_t N>
	RK4<N>::~RK4()
	{
	}

	/**
	 * Propagate for the specified interval
	 *
	 * @param[in]  t_now The current simulation time
	 * @param[in]  dt    Propagate for this long
	 * @param[in]  in    The input state vector, valid at \a t_now
	 * @param[out] out   The propagated state vector
	 *
	 * @return The next time to call this function, or -1
	 *         on error
	 */
	template <size_t N>
	int64 RK4<N>::propagate_to(int64 t_now,
		int64 dt,
		const Vector<N>& in,
		Vector<N>& out)
	{
		int64 t_next = t_now;

		for (int64 i = 0; i < dt; i++)
			t_next = step(t_now + i, in, out);

		return t_next;
	}

	/**
	 * Assign the function which computes the derivative of
	 * the state vector
	 *
	 * @return True on success
	 */
	template <size_t N>
	bool RK4<N>::register_dxdt(Function func)
	{
		if (!func) return false;
		_func = func;

		return true;
	}

	/**
	 * Take a single integration step
	 *
	 * @return The next time to call this function, or -1
	 *         on error
	 */
	template <size_t N>
	int64 RK4<N>::step(int64 t_now,
		const Vector<N>& in,
		Vector<N>& out)
	{
		if (!_func) return -1;

		const double t = t_now * _step_size;
		const double h = _step_size;

		auto k1 = h * _func(t, in);
		auto k2 = h * _func(t + h / 2, in + k1 / 2);
		auto k3 = h * _func(t + h / 2, in + k2 / 2);
		auto k4 = h * _func(t + h, in + k3);

		out = in +
			(k1 + 2 * k2 + 2 * k3 + k4) / 6;

		return t_now + 1;
	}
}
