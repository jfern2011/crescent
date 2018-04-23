#ifndef VECTOR_H
#define VECTOR_H

#include "Matrix.h"

namespace LinAlg
{
    /**
     ******************************************************************
     *
     * @class Vector
     *
     * Representation of a row or column vector
     *
     ******************************************************************
     */
    class Vector : public Matrix
    {
    public:
        /*
         **********************************************************
         *
         * Constructors
         *
         **********************************************************
         */
        Vector();
        Vector(const Vector&);
        Vector(const Matrix&);
        Vector(int, double);
        Vector(int, const double*);

        /*
         **********************************************************
         *
         * Destructor
         *
         **********************************************************
         */
        ~Vector();

        /*
         **********************************************************
         *
         * Operators (override base definitions)
         *
         **********************************************************
         */
        double& operator()(int) const;

        /*
         **********************************************************
         *
         * Member functions
         *
         **********************************************************
         */
        Vector cross(const Vector&) const;
        double dot(const Vector&) const;
        int length() const;
        double norm() const;
        Vector& resize(int, int);
        Matrix skew() const;
        Vector sub(int, int) const;
    };
}
#endif // VECTOR_H
