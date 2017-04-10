/**
 * tlibs test file
 * @author Tobias Weber <tobias.weber@tum.de>
 * @license GPLv2 or GPLv3
 */

// gcc -o larmor larmor.cpp -lstdc++ -std=c++11

#include "../math/mieze.h"

#include <iostream>
#include <boost/units/io.hpp>

namespace units = boost::units;
namespace codata = boost::units::si::constants::codata;


int main()
{
	tl::length len = 0.01 * tl::meters;
	tl::angle ang = -M_PI*tl::radians;
	tl::length lam = 2.36 * tl::angstrom;

	tl::flux B = tl::larmor_field(lam, len, ang);
	std::cout << double(B/tl::teslas*1000.) << " mT" << std::endl;


	tl::freq om = 2.*M_PI*64000. / tl::seconds;
	tl::flux B0 = tl::larmor_B(om);
	std::cout << double(B0/tl::teslas*1000.) << " mT" << std::endl;

	return 0;
}
