#include <cmath>
#include <csignal>
#include <iostream>

#include "Vector.h"

namespace LinAlg
{
    /**
     ******************************************************************
     *
     * Default constructor
     *
     ******************************************************************
     */
    Vector::Vector()
        : Matrix()
    {}

    /**
     ******************************************************************
     *
     * Copy constructor (1)
     *
     * Copy another vector
     *
     ******************************************************************
     */
    Vector::Vector(const Vector& other)
        : Matrix(other)
    {}

    /**
     ******************************************************************
     *
     * Copy constructor (2)
     *
     * Copy a vector from a row or column Matrix
     *
     ******************************************************************
     */
    Vector::Vector(const Matrix& other)
        : Matrix(other)
    {
        if (other.get_nCols() != 1 &&
            other.get_nRows() != 1)
        {
            std::cout << "Vector::Vector(): invalid dimensions ["
                << other.get_nRows() << ", "
                << other.get_nCols() << "] "
                << std::endl;
            std::raise(SIGABRT);
        }
    }

    /**
     ******************************************************************
     *
     * Construct a column vector of the specified dimension whose
     * elements are initialized to the given value
     *
     * @param[in] numel     The number of elements in the vector
     * @param[in] initValue A value assigned to all elements
     *
     ******************************************************************
     */
    Vector::Vector(int numel, double initValue)
        : Matrix(numel, 1, initValue)
    {}

    /**
     ******************************************************************
     *
     * Construct a column vector of the specified dimension whose
     * elements are assigned the values in the given array
     *
     * @param[in] numel The number of elements in the vector
     * @param[in] data  An array of values to copy into the vector
     *
     ******************************************************************
     */
    Vector::Vector(int numel, const double* data)
        : Matrix(numel, 1, data)
    {}

    /**
     ******************************************************************
     *
     * Destructor
     *
     ******************************************************************
     */
    Vector::~Vector()
    {}

    /**
     ******************************************************************
     *
     * Indexing operator
     *
     * @param[in] index An index into the vector
     *
     * @return A reference to the target element
     *
     ******************************************************************
     */
    double& Vector::operator()( int index ) const
    {
        if (index >= nRows * nCols)
        {
            std::cout << "Vector::operator(): index exceeds dimensions"
                << std::endl;
            std::raise(SIGABRT);
        }

        return data[index];
    }

    /**
     ******************************************************************
     *
     * Vector cross product. This operation is only defined for
     * 3-vectors
     *
     * @param[in] other The right-hand operand
     *
     * @return The cross product
     *
     ******************************************************************
     */
    Vector Vector::cross(const Vector& other) const
    {
        return Vector(skew() * other);
    }

    /**
     ******************************************************************
     *
     * Vector inner (dot) product
     *
     * @param[in] other The vector to take the dot product with
     *
     * @return The dot product
     *
     ******************************************************************
     */
    double Vector::dot(const Vector& other) const
    {
        register int i;
        double res = 0;

        if (length() != other.length())
        {
            std::cout << "Vector::dot(): operands have unequal lengths"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < length(); i++)
        {
            res += data[i]*other.data[i];
        }

        return(res);
    }

    /**
     ******************************************************************
     *
     * Get the number of elements in the vector
     *
     * @return The number of rows or columns, whichever is greater
     *
     ******************************************************************
     */
    int Vector::length() const
    {
        return nRows > nCols ? nRows : nCols;
    }

    /**
     ******************************************************************
     *
     * L2 norm
     *
     * @return The Euclidean norm of this vector
     *
     ******************************************************************
     */
    double Vector::norm() const
    {
        register int i, len = length();
        double res = 0;

        for ( i = 0; i < len; i++ )
        {
            res += data[i] * data[i];
        }

        return(std::sqrt(res));
    }

    /**
     ******************************************************************
     *
     * Resize a vector, whose new dimensions must be 1-by-n or n-by-1.
     * If this condition is not satisfied, this function does nothing
     *
     * @param[in] nrows The number of rows
     * @param[in] ncols The number of columns
     *
     * @return *this
     *
     ******************************************************************
     */
    Vector& Vector::resize( int nrows, int ncols )
    {
        if (nrows != 1 && ncols != 1)
        {
            return *this;
        }

        nRows = nrows;
        nCols = ncols;

        delete[] data;
            data = new double [ nrows * ncols ];

        return *this;
    }

    /**
     ******************************************************************
     *
     * Skew-symmetric matrix of a 3-vector
     *
     * @return The skew-symmetric matrix
     *
     ******************************************************************
     */
    Matrix Vector::skew() const
    {
        Matrix res(3,3,0.0);

        if (length() != 3)
        {
            std::cout << "Vector::skew(): not a 3-vector"
                << std::endl;
            std::raise(SIGABRT);
        }

        res(0,1) = -data[2];
        res(0,2) =  data[1];
        res(1,0) =  data[2];
        res(1,2) = -data[0];
        res(2,0) = -data[1];
        res(2,1) =  data[0];

        return(res);
    }

    /**
     ******************************************************************
     *
     * Retrieve a sub-vector
     *
     * @par
     * Similar to the Matrix sub() function, this will grab a sub-
     * vector from the specified starting index with the number of
     * elements to be extracted
     *
     * @param[in] index The starting index
     * @param[in] numel The number of elements to copy
     *
     * @return A sub-vector
     *
     ******************************************************************
     */
    Vector Vector::sub( int index, int numel ) const
    {
        if (numel <= 0 || index < 0 ||
            index + numel > length() )
        {
            std::cout << "Vector::sub(): exceeded vector dimensions"
                << std::endl;
            std::raise(SIGABRT);
        }

        return Vector( numel, &data[index] );
    }
}

