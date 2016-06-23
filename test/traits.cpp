#include <iostream>
#include <boost/units/systems/si.hpp>
#include "../log/debug.h"
#include "../helper/traits.h"

int main()
{
	std::cout << short(tl::get_scalar_type<double>::value) << std::endl;
	std::cout << tl::get_typename<tl::get_scalar_type<double>::value_type>() << std::endl;
	std::cout << std::endl;

	std::cout << short(tl::get_scalar_type<boost::units::dimensionless_quantity<boost::units::si::system, double>>::value) << std::endl;
	std::cout << tl::get_typename<tl::get_scalar_type<boost::units::dimensionless_quantity<boost::units::si::system, double>>::value_type>() << std::endl;

	return 0;
}
