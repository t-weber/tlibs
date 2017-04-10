/**
 * tlibs test file
 * @author Tobias Weber <tobias.weber@tum.de>
 * @license GPLv2 or GPLv3
 */

// gcc -I. -o mat0 mat0.cpp -std=c++11 -lstdc++ -lm

#include "../math/linalg.h"
#include <iostream>

using namespace tl;

int main()
{
	//std::cout << std::numeric_limits<double>::min() << std::endl;
	//std::cout << -std::numeric_limits<double>::max() << std::endl;

	ublas::matrix<double> mat1 = make_mat({{1., 2.}, {2.5, 3.}});
	ublas::matrix<double> mat2 = make_mat({{1., 2.}, {2., 3.}});

	double(*sqrt)(double) = std::sqrt;
	ublas::matrix<double> mat3 = apply_fkt(mat2, std::function<double(double)>(sqrt));

	std::cout << mat3 << std::endl;

	std::cout << is_symmetric(mat1) << std::endl;
	std::cout << is_symmetric(mat2) << std::endl;

	std::cout << get_minmax(mat1).first << std::endl;
	std::cout << get_minmax(mat1).second << std::endl;
	return 0;
}
