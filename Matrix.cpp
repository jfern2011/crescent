#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "Matrix.h"


namespace LinAlg
{
    // Results of intermediate computations:
    Matrix lastMatrixMult;
    Matrix lastMatrixSum;
    Matrix lastScalarMult;

    /**
     ******************************************************************
     *
     * Default constructor
     *
     ******************************************************************
     */
    Matrix::Matrix()
        : nRows(0), nCols(0), data(NULL)
    {}

    /**
     ******************************************************************
     *
     * Construct a matrix with the specified number of rows and columns
     * whose elements are initialized to the given value
     *
     * @param[in] nRows   The number of rows
     * @param[in] nCols   The number of columns
     * @param[in] initVal The initial value
     *
     ******************************************************************
     */
    Matrix::Matrix(int nRows, int nCols, double initVal)
        : nRows(0), nCols(0), data(NULL)
    {
        register int i, numel;

        if (nRows <= 0 || nCols <= 0)
        {
            std::cout << "Matrix::Matrix(): invalid dimensions ["
                << nRows << ", "
                << nCols << "] " << std::endl;
            std::raise(SIGABRT);
        }

        this->nRows = nRows;
        this->nCols = nCols;

        numel = nRows * nCols;

        data = new double[ numel ];

        for (i = 0; i < numel; i++)
        {
            data[i] = initVal;
        }
    }

    /**
     ******************************************************************
     *
     * Construct a matrix with the specified number of rows and columns
     * whose elements are assigned the values in the given array
     *
     * @param[in] nRows The number of rows
     * @param[in] nCols The number of columns
     * @param[in] data  A set of values to apply to the matrix given in
     *                  row-major order
     *
     ******************************************************************
     */
    Matrix::Matrix(int nRows, int nCols, const double* data)
        : nRows(0), nCols(0), data(NULL)
    {
        int numel;

        if (nRows <= 0 || nCols <= 0)
        {
            std::cout << "Matrix::Matrix(): invalid dimensions ["
                << nRows << ", "
                << nCols << "] " << std::endl;
            std::raise(SIGABRT);
        }

        this->nRows = nRows;
        this->nCols = nCols;

        numel = nRows * nCols;

        this->data = new double[ numel ];

        std::memcpy(this->data,
                    data, numel*sizeof(double));
    }

    /**
     ******************************************************************
     *
     * Copy constructor
     *
     ******************************************************************
     */
    Matrix::Matrix(const Matrix& other)
    {
        data = NULL;
        nRows = 0;
        nCols = 0;
        *this = other;
    }

    /**
     ******************************************************************
     *
     * Destructor
     *
     ******************************************************************
     */
    Matrix::~Matrix()
    {
        delete[] data;
    }

    /**
     ******************************************************************
     *
     * Assignment operator
     *
     * @param[in] Matrix to make *this a copy of
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::operator=(const Matrix& rhs)
    {
        if (this == &rhs) return *this;

        int numel = nRows * nCols;

        if (nRows != rhs.nRows || nCols != rhs.nCols)
        {
            nRows = rhs.nRows;
            nCols = rhs.nCols;

            delete[] data; data = NULL;

            numel = nRows*nCols;

            if (!numel)
                std::raise(SIGABRT);

            data = new double[ numel ];
        }

        std::memcpy(data, rhs.data,
                    numel*sizeof(double));

        return *this;
    }

    /**
     ******************************************************************
     *
     * Indexing operator
     *
     * @param[in] row Specifies the row to grab an element from
     * @param[in] col Specifies the column to grab an element from
     *
     * @return A reference to the target element
     *
     ******************************************************************
     */
    double& Matrix::operator()(int row, int col) const
    {
        if ( !(row < nRows && col < nCols)
            || row < 0
            || col < 0 )
        {
            std::cout << "Matrix::operator(): index exceeds dimensions"
                << std::endl;
            std::raise(SIGABRT);
        }
        return data[ row*nCols + col ];
    }

    /**
     ******************************************************************
     *
     * Element-wise addition (1)
     *
     * @param[in] m A matrix to add to this one
     *
     * @return The sum
     *
     ******************************************************************
     */
    Matrix Matrix::operator+(const Matrix& m) const
    {
        register int i, numel = nRows * nCols;

        lastMatrixSum = *this;

        if (nRows != m.nRows || nCols != m.nCols)
        {
            std::cout << "Matrix::operator+ : matrix dimensions disagree"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < numel; i++)
        {
            lastMatrixSum.data[i] += m.data[i];
        }

        return lastMatrixSum;
    }

    /**
     ******************************************************************
     *
     * Element-wise addition (2)
     *
     * @param[in] m A matrix to add to this one
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::operator+=(const Matrix& m)
    {
        register int i, numel = nRows * nCols;

        if (nRows != m.nRows || nCols != m.nCols)
        {
            std::cout << "Matrix::operator+ : matrix dimensions disagree"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < numel; i++)
        {
            this->data[i] += m.data[i];
        }

        return *this;
    }

    /**
     ******************************************************************
     *
     * Element-wise subtraction (1)
     *
     * @param[in] m A matrix to subtract from this one
     *
     * @return The difference
     *
     ******************************************************************
     */
    Matrix Matrix::operator-(const Matrix& m) const
    {
        register int i, numel = nRows * nCols;

        lastMatrixSum = *this;

        if (nRows != m.nRows || nCols != m.nCols)
        {
            std::cout << "Matrix::operator- : matrix dimensions disagree"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < numel; i++)
        {
            lastMatrixSum.data[i] -= m.data[i];
        }

        return lastMatrixSum;
    }

    /**
     ******************************************************************
     *
     * Element-wise subtraction (2)
     *
     * @param[in] m A matrix to subtract from this one
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::operator-=(const Matrix& m)
    {
        register int i, numel = nRows * nCols;

        if (nRows != m.nRows || nCols != m.nCols)
        {
            std::cout << "Matrix::operator- : matrix dimensions disagree"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < numel; i++)
        {
            this->data[i] -= m.data[i];
        }

        return *this;
    }

    /**
     ******************************************************************
     *
     * Multiplication by a scalar, with the scalar being the right-hand
     * operand (1)
     *
     * @param[in] x The scalar to multiply by
     *
     * @return The scalar-matrix product
     *
     ******************************************************************
     */
    Matrix Matrix::operator*( double x ) const
    {
        register int i, numel = nRows * nCols;
        lastScalarMult = *this;

        for (i = 0; i < numel; i++)
        {
            lastScalarMult.data[i] *= x;
        }

        return lastScalarMult;
    }

    /**
     ******************************************************************
     *
     * Multiplication by a scalar, with the scalar being the right-hand
     * operand (2)
     *
     * @param[in] x The scalar to multiply by
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::operator*=( double x )
    {
        register int i, numel = nRows * nCols;

        for (i = 0; i < numel; i++)
        {
            data[i] *= x;
        }

        return *this;
    }

    /**
     ******************************************************************
     *
     * Matrix multiplication (1)
     *
     * @param[in] m The matrix to multiply by
     *
     * @return The product
     *
     ******************************************************************
     */
    Matrix Matrix::operator*(const Matrix& m) const
    {
        Matrix res(nRows, m.nCols, 0.0);
        register int i, j, k;

        if (nCols != m.nRows)
        {
            std::cout << "Matrix::operator* : inner dimensions disagree"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < m.nCols; j++)
            {
                for (k = 0; k < m.nRows; k++)
                {
                    res(i,j) += (*this)(i,k) * m(k,j);
                }
            }
        }

        return res;
    }

    /**
     ******************************************************************
     *
     * Matrix multiplication (2)
     *
     * @param[in] m The matrix to multiply by
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::operator*=(const Matrix& m)
    {
        Matrix res(nRows, m.nCols, 0.0);
        register int i, j, k;

        if (nCols != m.nRows)
        {
            std::cout << "Matrix::operator* : inner dimensions disagree"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < m.nCols; j++)
            {
                for (k = 0; k < m.nRows; k++)
                {
                    res(i,j) += (*this)(i,k) * m(k,j);
                }
            }
        }

        return *this = res;
    }

    /**
     ******************************************************************
     *
     * Multiplication by a scalar, with the scalar being the left-hand
     * operand
     *
     * @param[in] x The scalar to multiply by
     * @param[in] m The matrix being multiplied
     *
     * @return The matrix-scalar product
     *
     ******************************************************************
     */
    Matrix operator*(double x, const Matrix& m)
    {
        return(m * x);
    }

    /**
     ******************************************************************
     *
     * Division by a scalar (1)
     *
     * @param[in] x The scalar to divide by
     *
     * @return The quotient
     *
     ******************************************************************
     */
    Matrix Matrix::operator/( double x ) const
    {
        register int i, numel = nRows * nCols;
        lastScalarMult = *this;

        if (x == 0.0)
        {
            std::cout << "Matrix::operator/ : divide by zero occurred"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < numel; i++)
        {
            lastScalarMult.data[i] /= x;
        }

        return lastScalarMult;
    }

    /**
     ******************************************************************
     *
     * Division by a scalar (2)
     *
     * @param[in] x The scalar to divide by
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::operator/=( double x )
    {
        register int i, numel = nRows * nCols;

        if (x == 0.0)
        {
            std::cout << "Matrix::operator/ : divide by zero occurred"
                << std::endl;
            std::raise(SIGABRT);
        }

        for (i = 0; i < numel; i++)
        {
            data[i] /= x;
        }

        return *this;
    }

    /**
     ******************************************************************
     *
     * Comparison operator
     *
     * Performs element-wise comparison to determine if two matrices
     * are equal
     *
     * @param[in] rhs The matrix to compare to
     *
     * @return True if both are equal
     *
     ******************************************************************
     */
    bool Matrix::operator==(const Matrix& rhs ) const
    {
        register int i, numel = nRows * nCols;

        if (nRows != rhs.nRows || nCols != rhs.nCols)
        {
            return false;
        }

        for (i = 0; i < numel; i++)
        {
            if (data[i] != rhs.data[i])
            {
                return false;
            }
        }

        return true;
    }

    /**
     ******************************************************************
     *
     * @return The number of columns in this matrix
     *
     ******************************************************************
     */
    int Matrix::get_nCols(void) const
    {
        return nCols;
    }

    /**
     ******************************************************************
     *
     * @return The number of rows in this matrix
     *
     ******************************************************************
     */
    int Matrix::get_nRows(void) const
    {
        return nRows;
    }

    /**
     ******************************************************************
     *
     * Set this matrix to the identity, preserving its dimensions
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::identify(void)
    {
        register int i;

        if (nRows != nCols)
        {
            std::cout << "Matrix::identify(): matrix is not square"
                << std::endl;
            std::raise(SIGABRT);
        }

        zeroify();

        for (i = 0; i < nRows; i++) {
            data[i * (nCols+1)]= 1.0;
        }

        return *this;
    }

    /**
     ******************************************************************
     *
     * Check if the matrix is square
     *
     * @return True if this is a square matrix
     *
     ******************************************************************
     */
    bool Matrix::isSquare(void) const
    {
        return nRows == nCols;
    }

    /**
     ******************************************************************
     *
     * Print this matrix to standard output
     *
     ******************************************************************
     */
    void Matrix::print(void) const
    {
        int i, j;
        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                std::printf("%14.6f ", (*this)(i,j));
            }
            std::printf("\n");
        }
        std::fflush(stdout);
    }

    /**
     ******************************************************************
     *
     * Resize a matrix. Values from the old matrix are not guaranteed
     * preserved
     *
     * @param[in] nrows The new number of rows
     * @param[in] ncols The new number of columns
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::resize(int nrows, int ncols)
    {
        nRows = nrows;
        nCols = ncols;

        delete[] data;
            data = new double [ nrows * ncols ];

        return *this;
    }

    /**
     ******************************************************************
     *
     * Re-populate the matrix with new data
     *
     * @param[in] data The set of values to be assigned to this matrix
     *                 in row-major order
     *
     ******************************************************************
     */
    void Matrix::setData(const double* data)
    {
        std::memcpy(this->data, data,
            nRows * nCols * sizeof(double));
    }

    /**
     ******************************************************************
     *
     * Grab a block matrix from this matrix, specifying the starting
     * row and column and indicating the dimensions of the sub-block to
     * be extracted
     *
     * @param[in] row   The starting row number
     * @param[in] col   The starting column
     * @param[in] nRows The number of rows to copy
     * @param[in] nCols The number of columns to copy
     *
     * @return A sub-matrix
     *
     ******************************************************************
     */
    Matrix Matrix::sub(int row, int col, int nRows, int nCols) const
    {
        register int i, j;

        if ( nRows <= 0 || nCols <= 0 || row < 0 || col < 0
            || (row+nRows) > this->nRows
            || (col+nCols) > this->nCols )
        {
            std::cout << "Matrix::sub(): exceeded matrix dimensions"
                << std::endl;
            std::raise(SIGABRT);
        }

        Matrix res(nRows, nCols, 0.0);

        for (i = 0; i < nRows; i++)
        {
            for (j = 0; j < nCols; j++)
            {
                res(i,j) = (*this)(i+row, j+col);
            }
        }

        return res;
    }

    /**
     ******************************************************************
     *
     * Matrix transpose
     *
     * @return The transpose of *this
     *
     ******************************************************************
     */
    Matrix Matrix::transpose(void) const
    {
        register int i, j;
        Matrix res;
        double element;

        if (nCols != nRows)
        {
            res = Matrix( nCols, nRows, this->data );
            for (i = 0; i < nRows; i++)
            {
                for (j = 0; j < nCols; j++)
                {
                    res(j,i) = (*this)(i,j);
                }
            }
        }
        else
        {
            res = *this;
            for (i = 0; i < nRows; i++)
            {
                for (j = i+1; j < nCols; j++)
                {
                    element = res(i,j); res(i,j) = res(j,i);
                    res(j,i) =  element;
                }
            }
        }
        return res;
    }

    /**
     ******************************************************************
     *
     * Set the elements of this matrix to zero
     *
     * @return *this
     *
     ******************************************************************
     */
    Matrix& Matrix::zeroify(void)
    {
        std::memset(data, 0, nRows*nCols*sizeof(double));
        return *this;
    }
}
