#ifndef __RK4_H__
#define __RK4_H__

#include "Event.h"
#include "crescent.h"
#include "Vector.h"

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
	 *
	 * @tparam C The class that implements this function
	 */
	template <typename C>
	using Function =
		Vector<N>(C::*)(double time, const Vector<N>&);

	RK4(double step_size);

	~RK4();

	int64 propagate_to(int64 t_now,
					   int64 dt,
					   const Vector<N>& in,
					   Vector<N>& out);

	bool register_dxdt(C& obj, Function func);

	int64 step(int64 t_now,
			   const Vector<N>& in,
			   Vector<N>& out);

private:

	/**
	 * The object through which to invoke \ref _func
	 */
	C& _obj;

	/**
	 * Computes the derivative dx/dt
	 */
	Function _func;

	/**
	 * The integration step size (sec)
	 */
	double _step_size;
};

#endif

template <size_t N>
RK4<N>::RK4(double step_size)
	: _cycle(0), _step_size(step_size)
{
}

template <size_t N>
RK4<N>::~RK4()
{
}

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

template <typename C> template <size_t N>
bool RK4<N>::register_dxdt(C& obj, Function func)
{
	if (!func) return false;

	_func = func; _obj = obj;
	return true;
}

template <size_t N>
int64 RK4<N>::step(int64 t_now,
				   const Vector<N>& in,
				   Vector<N>& out)
{
	if (!_func) return -1;

	const double t  = t_now * _step_size;
	const double h  = _step_size;

	auto k1 = h * (_obj.*_func)(t, in);
	auto k2 = h * (_obj.*_func)(t + h / 2, in + k1 / 2);
	auto k3 = h * (_obj.*_func)(t + h / 2, in + k2 / 2);
	auto k4 = h * (_obj.*_func)(t + h, in + k3);

	out = in +
		(k1 + 2 * k2 + 2 * k3 + k4) / 6;

	return t_now + 1;
}
