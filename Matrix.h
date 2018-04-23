#ifndef MATRIX_H
#define MATRIX_H

namespace LinAlg
{
    /**
     ******************************************************************
     *
     * @class Matrix
     *
     * Matrix class implementation. Matrices here are in row-major
     * order
     *
     ******************************************************************
     */
    class Matrix
    {
    public:
        /*
         **********************************************************
         *
         * Constructors
         *
         **********************************************************
         */
        Matrix();
        Matrix(int, int, double);
        Matrix(int, int, const double*);
        Matrix(const Matrix&);

        /*
         **********************************************************
         *
         * Destructor
         *
         **********************************************************
         */
        ~Matrix();

        /*
         **********************************************************
         *
         * Overloaded operators
         *
         **********************************************************
         */
        Matrix& operator=(const Matrix&);
        double& operator()(int, int) const;
        Matrix  operator+(const Matrix&) const;
        Matrix& operator+=(const Matrix&);
        Matrix  operator-(const Matrix&) const;
        Matrix& operator-=(const Matrix&);
        Matrix  operator*(double) const;
        Matrix& operator*=(double);
        Matrix operator*(const Matrix&) const;
        Matrix& operator*=(const Matrix&);
        friend Matrix operator*(double, const Matrix&);
        Matrix  operator/(double) const;
        Matrix& operator/=(double);
        bool operator==(const Matrix&) const;

        /*
         **********************************************************
         *
         * Member functions
         *
         **********************************************************
         */
        int get_nCols(void) const;
        int get_nRows(void) const;
        Matrix& identify(void);
        bool isSquare(void) const;
        void print(void) const;
        Matrix& resize(int, int);
        void setData(const double*);
        Matrix sub(int, int, int, int) const;
        Matrix transpose(void) const;
        Matrix& zeroify(void);

    protected:
        int nRows, nCols;
        double* data;
    };
}

#endif // MATRIX_H
