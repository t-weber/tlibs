// gcc -o quat quat.cpp -std=c++11 -lstdc++ -lm

#include "../math/quat.h"
#include <iostream>

using namespace tl;
typedef float T;

int main()
{
	ublas::matrix<T> mat = rotation_matrix_3d_x(1.23);
	std::cout << "mat = " << mat << std::endl;

	math::quaternion<T> quat = rot3_to_quat(mat);
	std::cout << "quat = " << quat << std::endl;

	mat = quat_to_rot3(quat);
	std::cout << "mat = " << mat << std::endl;

	return 0;
}
