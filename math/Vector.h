#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cmath>

#include "Matrix.h"

/**
 * @class Vector
 *
 * Represents an Nx1 (column) vector. To work with row vectors, see
 * \ref Matrix<1,N>
 */
template <size_t N>
class Vector : public Matrix<N,1>
{

public:

	Vector(double init_val = 0);

	Vector(const Matrix<N,1>& mat);

	Vector(const double* data);

	~Vector();

	Vector& operator=(const Matrix<N,1>& mat);

	double& operator()(size_t index);

	double operator()(size_t index) const;

	Vector cross(const Vector& rhs) const;

	double dot(const Vector& rhs) const;

	size_t length() const;

	double norm() const;

	Matrix<3,3> skew() const;

	template <size_t M>
		Vector< M > sub(size_t index) const;

	template <size_t M>
	Vector<N + M> vcat(const Vector<M>& vec)
		const;
};

/**
 * Constructor (1)
 *
 * @param[in] init_val Initialize by setting all entries to this
 */
template <size_t N>
inline Vector<N>::Vector(double init_val) : Matrix<N,1>(init_val)
{
}

/**
 * Constructor (2)
 *
 * @param [in] data Initialize the vector by pulling values from
 *                  this
 */
template <size_t N>
inline Vector<N>::Vector(const double* data) : Matrix<N,1>(data)
{
}

/**
 * Copy constructor
 *
 * @param[in] mat The \ref Matrix or Nx1 vector to copy
 */
template <size_t N>
inline Vector<N>::Vector(const Matrix<N,1>& mat)
	: Matrix<N,1>(mat)
{
}

/**
 * Destructor
 */
template <size_t N>
inline Vector<N>::~Vector()
{
}

/**
 * Assignment operator
 *
 * @param[in] mat The \ref Matrix or Vector to assign to
 *
 * @return *this
 */
template <size_t N>
inline auto Vector<N>::operator=(const Matrix<N,1>& mat) -> Vector&
{
	if (this != &mat)
		Matrix<N,1>::operator=(mat);

	return *this;
}

/**
 * Indexing operator
 *
 * @param[in] index Get the element at this index
 *
 * @return The element at index \a index
 */
template <size_t N>
inline double& Vector<N>::operator()(size_t index)
{
	if (N <= index)
	{
		char msg[256];
		std::snprintf(msg, 256, 
			"%s in %s, line %d: N = %zu, index = %zu\n",
			__PRETTY_FUNCTION__, __FILE__, __LINE__,
			N, index);

		throw std::out_of_range( msg );
	}

	return this->_data[index];
}

/**
 * Indexing operator (const overload)
 *
 * @param[in] index Get the element at this index
 *
 * @return The element at index \a index
 */
template <size_t N>
inline double Vector<N>::operator()(size_t index) const
{
	if (N <= index)
	{
		char msg[256];
		std::snprintf(msg, 256, 
			"%s in %s, line %d: N = %zu, index = %zu\n",
			__PRETTY_FUNCTION__, __FILE__, __LINE__,
			N, index);

		throw std::out_of_range( msg );
	}

	return this->_data[index];
}

/**
 * Vector cross product. This operation is only defined for
 * 3-vectors
 *
 * @param[in] rhs The right-hand operand
 *
 * @return The cross product
 */
template <size_t N>
inline auto Vector<N>::cross(const Vector& rhs) const -> Vector
{
	static_assert(N == 3,
		"cross product is only defined for 3x1 vectors.");

	return skew() * rhs;
}

/**
 * Vector dot product
 *
 * @param[in] rhs The vector to take the dot product with
 *
 * @return The dot product
 */
template <size_t N>
inline double Vector<N>::dot(const Vector& rhs) const
{
	double ans = 0;

	for (int i = 0; i < N; i++)
    {
        ans += this->_data[i] * rhs._data[i];
    }

    return ans;
}

/**
 * Get the number of elements in the vector
 *
 * @return The number of rows
 */
template <size_t N>
inline size_t Vector<N>::length() const
{
	return N;
}

/**
 * Compute the L^2 (Euclidean) norm
 *
 * @return The vector norm
 */
template <size_t N>
inline double Vector<N>::norm() const
{
	return std::sqrt(dot(*this));
}

/**
 * Compute the skew-symmetric matrix of a 3-vector
 *
 * @return The skew-symmetric matrix
 */
template <size_t N>
inline Matrix<3,3> Vector<N>::skew() const
{
	static_assert(N == 3,
		"skew() is only defined for 3x1 vectors.");

	Matrix<3,3> ans;

	ans(0,1) = -this->_data[2];
    ans(0,2) =  this->_data[1];
	ans(1,0) =  this->_data[2];
	ans(1,2) = -this->_data[0];
	ans(2,0) = -this->_data[1];
	ans(2,1) =  this->_data[0];

	return ans;
}

/**
 * Retrieve a sub-vector
 *
 * @param[in] index The starting index from which to grab the
 *                  smaller vector
 *
 * @return A sub-vector
 */
template <size_t N> template <size_t M>
inline Vector<M> Vector<N>::sub(size_t index) const
{
	static_assert(M <= N,
		"cannot take a sub-vector of a larger one.");

	if (M + index > N)
	{
		char msg[256];
		std::snprintf(msg, 256, 
			"%s in %s, line %d: N = %zu, M = %zu, index = %zu\n",
			__PRETTY_FUNCTION__, __FILE__, __LINE__,
			N, M, index);

		throw std::out_of_range( msg );
	}

	Vector<M> ans(&this->_data[index]);

	return ans;
}

/**
 * Concatenate two Vectors
 *
 * @param[in] vec The Vector which to append to *this
 *
 * @return The concatenated vector
 */
template <size_t N> template <size_t M>
inline Vector<N + M> Vector<N>::vcat(const Vector<M>& vec) const
{
	Vector<N + M> ans;

	for (size_t n = 0; n < N; n++)
		ans(n) = this->_data[n];

	for (size_t m = 0; m < M; m++)
		ans(N+m) = vec._data[m];

	return ans;
}

#endif
