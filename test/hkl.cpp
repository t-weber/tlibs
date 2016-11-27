// gcc -o hkl hkl.cpp ../log/log.cpp -std=c++11 -lstdc++ -lm

#include "../math/lattice.h"

using T = double;
using t_vec = tl::ublas::vector<T>;

int main()
{
	T dTh=0., d2Th=0., dChi=0., dPsi=0.;
	tl::Lattice<T> lat(5.,5.,5., M_PI/2., M_PI/2., M_PI/2.);

	tl::get_euler_angles(lat,
		//tl::make_vec<t_vec>({1., 0., 0.}), tl::make_vec<t_vec>({1., 1., 0.}),
		1.4, 1., 0., 1.,
		&dTh, &d2Th, &dChi, &dPsi);

	tl::log_info("2theta = ", tl::r2d(d2Th));
	tl::log_info("theta = ", tl::r2d(dTh));
	tl::log_info("chi = ", tl::r2d(dChi));
	tl::log_info("psi = ", tl::r2d(dPsi));

	return 0;
}
