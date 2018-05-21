#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <cstddef>
#include <stdexcept>

#include "abort.h"

/**
 * @class Matrix
 *
 * Represents an NxM matrix. Elements are placed in row-major order
 */
template <size_t N, size_t M>
class Matrix
{

public:

	Matrix(double init_val = 0);

	Matrix(const double* data);

	virtual ~Matrix() {};

    double& operator()(size_t row, size_t col);

    double  operator()(size_t row, size_t col) const;

	Matrix  operator+(const Matrix& rhs) const;

	Matrix& operator+=(const Matrix& rhs);

	Matrix  operator-(const Matrix& rhs) const;

	Matrix& operator-=(const Matrix& rhs);

	Matrix  operator*(double scale) const;

	Matrix& operator*=(double scale);

	Matrix  operator/(double scale) const;

	Matrix& operator/=(double scale);

	template <size_t C>
	Matrix operator*(const Matrix<M,C>& rhs) const;

	template <size_t C>
	Matrix& operator*=(const Matrix<M,C>& rhs);

	bool operator==( const Matrix& rhs ) const;

	size_t ncols() const;

	size_t nrows() const;

	Matrix& identify();

	Matrix& zeroify();

	bool set_data(const double* data);

	template <size_t R, size_t C>
	Matrix<R,C> sub(size_t row, size_t col) const;

	Matrix<M,N> transpose() const;

protected:

	/**
	 * The underlying buffer
	 */
	double _data[N*M];
};

/**
 * Constructor (1)
 *
 * @param[in] init_val Initialize by setting all entries to this
 */
template <size_t N, size_t M>
inline Matrix<N,M>::Matrix( double init_val )
{
	for (size_t i = 0; i < N * M; i++)
		_data[i] = init_val;
}

/**
 * Constructor (2)
 *
 * @param [in] data Initialize the matrix by pulling values from
 *                  this in row-major order
 */
template <size_t N, size_t M>
inline Matrix<N,M>::Matrix(const double* data)
{
	if (data == nullptr)
	{
		throw std::invalid_argument( "null pointer exception!" );
	}

	for (size_t i = 0; i < N * M; i++)
		_data[i] = data[i];
}

/**
 * Indexing operator
 *
 * @param[in] row Get the element at this row
 * @param[in] col Get the element at this column
 *
 * @return A reference to the target element
 */
template <size_t N, size_t M>
inline double& Matrix<N,M>::operator()(size_t row, size_t col)
{
	if (N <= row || M <= col)
	{
		char msg[256];
		std::snprintf(msg, 256, 
			"%s in %s, line %d: <N,M> = <%zu,%zu>, (row,col) = (%zu,%zu)\n",
			__PRETTY_FUNCTION__, __FILE__, __LINE__,
			N, M, row, col);

		throw std::out_of_range( msg );
	}

	return _data[row * M + col];
}

/**
 * Indexing operator (const overload)
 *
 * @param[in] row Get the element at this row
 * @param[in] col Get the element at this column
 *
 * @return A reference to the target element
 */
template <size_t N, size_t M>
inline double Matrix<N,M>::operator()(size_t row, size_t col) const
{
	if (N <= row || M <= col)
	{
		char msg[256];
		std::snprintf(msg, 256, 
			"%s in %s, line %d: <N,M> = <%zu,%zu>, (row,col) = (%zu,%zu)\n",
			__PRETTY_FUNCTION__, __FILE__, __LINE__,
			N, M, row, col);

		throw std::out_of_range( msg );
	}

	return _data[row * M + col];
}

/**
 * Element-wise addition (1)
 *
 * @param[in] rhs The matrix to add to *this
 *
 * @return The matrix sum
 */
template <size_t N, size_t M>
inline auto Matrix<N,M>::operator+(const Matrix& rhs) const -> Matrix
{
	Matrix<N,M> temp;

	for (size_t i = 0; i < N*M; i++)
	{
		temp._data[i] = _data[i] + rhs._data[i];
	}

	return temp;
}

/**
 * Element-wise addition (2)
 *
 * @param[in] rhs The matrix to add to *this
 *
 * @return *this
 */
template<size_t N, size_t M>
inline auto Matrix<N,M>::operator+=(const Matrix& rhs) -> Matrix&
{
	for (size_t i = 0; i < N*M; i++)
		_data[i] += rhs._data[i];

	return *this;
}

/**
 * Element-wise subtraction (1)
 *
 * @param[in] rhs The matrix to subtract from *this
 *
 * @return The matrix difference
 */
template <size_t N, size_t M>
inline auto Matrix<N,M>::operator-(const Matrix& rhs) const-> Matrix
{
	Matrix<N,M> temp;

	for (size_t i = 0; i < N*M; i++)
	{
		temp._data[i] = _data[i] - rhs._data[i];
	}

	return temp;
}

/**
 * Element-wise subtraction (2)
 *
 * @param[in] rhs The matrix to subtract from *this
 *
 * @return *this
 */
template<size_t N, size_t M>
inline auto Matrix<N,M>::operator-=(const Matrix& rhs) -> Matrix&
{
	for (size_t i = 0; i < N*M; i++)
		_data[i] -= rhs._data[i];

	return *this;
}

/**
 * Multiplication by a scalar, with the scalar as the right-hand
 * operand (1)
 *
 * @param[in] scale Scale the matrix by this amount
 *
 * @return The scalar-matrix product
 */
template <size_t N, size_t M>
inline auto Matrix<N,M>::operator*(double scale) const -> Matrix
{
	Matrix<N,M> temp;

	for (size_t i = 0; i < N*M; i++)
	{
		temp._data[i] = _data[i] * scale;
	}

	return temp;
}

/**
 * Multiplication by a scalar, with the scalar as the right-hand
 * operand (2)
 *
 * @param[in] scale Scale the matrix by this amount
 *
 * @return *this
 */
template <size_t N, size_t M>
inline auto Matrix<N,M>::operator*=(double scale) -> Matrix&
{
	for (size_t i = 0; i < N*M; i++)
		_data[i] *= scale;

	return *this;
}

/**
 * Division by a scalar (1)
 *
 * @param[in] scale The scalar to divide by
 *
 * @return The element-wise quotient
 */
template <size_t N, size_t M>
inline auto Matrix<N,M>::operator/(double scale) const -> Matrix
{
	Matrix<N,M> temp;

	if (scale == 0.0)
	{
		throw std::overflow_error( "attempted to divide by zero!" );
	}

	for (size_t i = 0; i < N*M; i++)
	{
		temp._data[i] = _data[i] / scale;
	}

	return temp;
}

/**
 * Division by a scalar (2)
 *
 * @param[in] scale The scalar to divide by
 *
 * @return *this
 */
template <size_t N, size_t M>
inline auto Matrix<N,M>::operator/=(double scale) -> Matrix&
{
	if (scale == 0.0)
	{
		throw std::overflow_error( "attempted to divide by zero!" );
	}

	for (size_t i = 0; i < N*M; i++)
		_data[i] /= scale;

	return *this;
}

/**
 * Multiplication by a scalar, with the scalar as the left-hand
 * operand
 *
 * @param[in] scale Scale the matrix by this amount
 * @param[in] rhs   The matrix being scale
 *
 * @return The matrix-scalar product
 */
template <size_t N, size_t M>
inline Matrix<N,M> operator*(double scale, const Matrix<N,M>& rhs)
{
	return (rhs * scale);
}

/**
 * Matrix multiplication (1)
 *
 * @tparam C The number of columns in the right-hand operand
 *
 * @param [in] rhs Multiply by this (right-hand side) matrix
 *
 * @return The matrix product
 */
template <size_t N, size_t M> template <size_t C>
inline auto Matrix<N,M>::operator*(const Matrix<M,C>& rhs)
	const -> Matrix
{
	Matrix<N,C> temp;
	for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < C; j++)
        {
            for (size_t k = 0; k < M; k++)
            {
                temp(i,j) += this->operator()(i,k) * rhs(k,j);
            }
        }
    }

	return temp;
}

/**
 * Matrix multiplication (2)
 *
 * @tparam C The number of columns in the right-hand operand
 *
 * @param [in] rhs Multiply by this (right-hand side) matrix
 *
 * @return *this
 */
template <size_t N, size_t M> template <size_t C>
inline auto Matrix<N,M>::operator*=(const Matrix<M,C>& rhs)
	-> Matrix&
{
	return (*this = operator*(rhs));
}

/**
 * Performs element-wise comparison to determine if two matrices
 * are equal
 *
 * @param[in] rhs The matrix to compare against
 *
 * @return True if both are equal
 */
template <size_t N, size_t M>
inline bool Matrix<N,M>::operator==(const Matrix& rhs) const
{
	for (size_t i = 0 ; i < N*M; i++)
	{
		if (_data[i] != rhs._data[i])
			return false;
	}

	return true;
}

/**
 * Get the number of columns in this matrix
 *
 * @return The number of columns
 */
template <size_t N, size_t M>
inline size_t Matrix<N,M>::ncols() const
{
	return M;
}

/**
 * Get the number of rows in this matrix
 *
 * @return The number of rows
 */
template <size_t N, size_t M>
inline size_t Matrix<N,M>::nrows() const
{
	return N;
}

/**
 * Set this matrix to the identity, preserving its dimensions
 *
 * @return *this
 */
template <size_t N, size_t M>
inline auto Matrix<N,M>::identify() -> Matrix&
{
	static_assert(N == M, "matrix must be square");

	zeroify();

	for (size_t i = 0; i < N; i++)
		this->operator()(i,i) = 1.0;

	return *this;
}

/**
 * Set the elements of this matrix to zero
 *
 * @return *this
 */
template <size_t N, size_t M>
inline auto Matrix<N,M>::zeroify() -> Matrix&
{
	for (size_t i = 0; i < N * M; i++)
		_data[i] = 0.0;

	return *this;
}

/**
 * Re-assign the matrix data
 *
 * @param[in] data The set of values to be assigned to this matrix
 *                 in row-major order
 *
 * @return True on success
 */
template <size_t N, size_t M>
inline bool Matrix<N,M>::set_data(const double* data)
{
#ifndef _WIN32
	AbortIfNot(data, false);
#endif

	for (size_t i = 0; i < N*M; i++)
		_data[i] = data[i];

	return true;
}

/**
 * Grab a block matrix from this matrix
 *
 * @tparam R The number of rows in the sub-matrix
 * @tparam C The number of columns
 *
 * @param[in] row   The starting row of the block
 * @param[in] col   The starting column
 *
 * @return A sub-matrix
 */
template <size_t N, size_t M> template <size_t R, size_t C>
inline Matrix<R,C> Matrix<N,M>::sub(size_t row, size_t col) const
{
	static_assert(R < N && C < M, "");

	if (row + R > N || col + C > M)
	{
		char msg[256];
		std::snprintf(msg, 256,
			"%s in %s, line %d: (row,col) = (%zu,%zu), <R,C> = <%zu,%zu>\n",
			__PRETTY_FUNCTION__, __FILE__, __LINE__,
			row, col, R, C);

		throw std::out_of_range( msg );
	}

	Matrix<R,C> temp;

	for (size_t i = 0; i < R; i++)
	{
		for (size_t j = 0; j < C; j++)
		{
			temp(i,j) =
				this->operator()(i+row, j+col);
		}
	}

	return temp;
}

/**
 * Compute the transpose of this matrix
 *
 * @return The transpose of *this
 */
template <size_t N, size_t M>
inline Matrix<M,N> Matrix<N,M>::transpose() const
{
	Matrix<M,N> ans;

	for (size_t i = 0; i < N; i++)
	{
		for (size_t j = 0; j < M; j++)
		{
			ans(j,i) = this->operator()(i,j);
		}
	}

	return ans;
}

#endif
