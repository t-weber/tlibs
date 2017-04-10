/**
 * tlibs test file
 * @author Tobias Weber <tobias.weber@tum.de>
 * @license GPLv2 or GPLv3
 */

// gcc -o traits traits.cpp -lstdc++ -std=c++14

#include <iostream>
#include <boost/units/systems/si.hpp>
#include "../log/debug.h"
#include "../helper/traits.h"


template<typename T> using t_arr = std::array<T, 3>;

int main()
{
	std::cout << short(tl::get_scalar_type<double>::value) << std::endl;
	std::cout << tl::get_typename<tl::get_scalar_type<double>::value_type>() << std::endl;
	std::cout << std::endl;

	std::cout << short(tl::get_scalar_type<boost::units::dimensionless_quantity<boost::units::si::system, double>>::value) << std::endl;
	std::cout << tl::get_typename<tl::get_scalar_type<boost::units::dimensionless_quantity<boost::units::si::system, double>>::value_type>() << std::endl;
	std::cout << std::endl;

	std::vector<int> vec({2,4,6});
	std::cout << tl::call<3>([](int a, int b, int c)->int{ return a*(b+c); }, vec) << std::endl;

	t_arr<int> arr({2,4,6});
	std::cout << tl::call<arr.size(), int(int,int,int), int, t_arr>
		([](int a, int b, int c)->int{ return a*(b+c); }, arr) << std::endl;

	return 0;
}
