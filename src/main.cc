#include <cstdlib>
#include <iostream>

#include "Eigen/Dense"

int main(int argc, char** argv) {
    Eigen::MatrixXd matrix(2,2);

    matrix(0,0) = 3.0;
    matrix(1,0) = 2.5;
    matrix(0,1) = -1.0;
    matrix(1,1) = matrix(1,0) + matrix(0,1);

    std::cout << matrix << std::endl;
    return EXIT_SUCCESS;
}
