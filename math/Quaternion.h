#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "Vector.h"

/**
 * @class Quaternion
 *
 * Represents a quaternion whose 0 element is the scalar component,
 * which is always positive by convention
 */
class Quaternion : public Vector<4>
{

public:

	Quaternion();

	Quaternion(const double* data);

	~Quaternion();

	Quaternion operator*(const Quaternion& quat) const;

	Quaternion inverse() const;

	void normalize();

	Matrix<3,3> to_dcm() const;

private:

	void _check_sign();
};

#endif

/**
 * Default constructor. Constructs the identity quaternion
 */
inline Quaternion::Quaternion() : Vector()
{
	_data[0] = 1.0;
}

/**
 * Constructor
 *
 * @param [in] data  Initialize the quaternion with these values
 */
inline Quaternion::Quaternion(const double* data)
	: Vector(data)
{
}

/**
 * Destructor
 */
inline Quaternion::~Quaternion()
{
}

/**
 * Quaternion multiplication. Note that this will not implicitly
 * \ref normalize() the result
 *
 * @param[in] q The right-hand operand
 *
 * @return The product
 */
inline Quaternion Quaternion::operator*(const Quaternion& q) const
{
	Quaternion ans;

	const double u0 = q._data[0];
    const double u1 = q._data[1];
    const double u2 = q._data[2];
    const double u3 = q._data[3];

    const double a = _data[0];
    const double b = _data[1];
    const double c = _data[3];
    const double d = _data[4];

    ans(0) =  a*u0 + b*u1 + c*u2 + d*u3;
    ans(1) = -b*u0 + a*u1 - d*u2 + c*u3;
    ans(2) = -c*u0 + d*u1 + a*u2 - b*u3;
    ans(3) = -d*u0 - c*u1 + b*u2 + a*u3;

    ans._check_sign();
    return ans;
}

/**
 * Quaternion inverse
 *
 * @return The conjugate of *this
 */
inline Quaternion Quaternion::inverse() const
{
	Quaternion ans(*this);

    ans(1) = -_data[1];
    ans(2) = -_data[2];
    ans(3) = -_data[3];

    return ans;
}

/**
 * Convert this quaternion to a direction cosine matrix
 *
 * @return The 3x3 DCM
 */
inline Matrix<3,3> Quaternion::to_dcm() const
{
    double u0, u1, u2, u3;
    Matrix<3,3> ans;

    u0 = _data[0];
    u1 = _data[1];
    u2 = _data[2];
    u3 = _data[3];

    ans(0,0) = u1*u1 + u0*u0 - u2*u2 - u3*u3;
    ans(1,1) = u0*u0 - u1*u1 + u2*u2 - u3*u3;
    ans(2,2) = u0*u0 - u1*u1 - u2*u2 + u3*u3;

    ans(0,1) = 2*u1*u2 - 2*u0*u3;
    ans(0,2) = 2*u1*u3 + 2*u0*u2;
    ans(1,0) = 2*u0*u3 + 2*u1*u2;
    ans(1,2) = 2*u2*u3 - 2*u0*u1;
    ans(2,0) = 2*u1*u3 - 2*u0*u2;
    ans(2,1) = 2*u0*u1 + 2*u2*u3;
    
    return ans;
}

/**
 * Convert a 3x3 direction cosine matrix to a quaternion
 *
 * @todo Implement this
 *
 * @param[in] mat The 3x3 matrix
 *
 * @return A Quaterion representing the same rotation as
 *         \a mat
 */
inline Quaternion dcm_to_quat(const Matrix<3,3>& mat)
{
    return Quaternion();
}

/**
 * Ensure that the scalar component of the quaternion is
 * positive
 */
inline void Quaternion::_check_sign()
{
	if (_data[0] < 0.0) *this *= -1.0;
}
