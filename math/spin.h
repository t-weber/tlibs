/**
 * spins
 * @author: Tobias Weber <tobias.weber@tum.de>
 * @date: 2016
 * @license GPLv2 or GPLv3
 */

#ifndef __TLIBS_SPIN_H__
#define __TLIBS_SPIN_H__

#include "linalg.h"

namespace tl {

/**
 * spin matrices
 * see e.g. (Arfken 2013), p. 110
 */
template<template<class...> class t_mat=ublas::matrix,
	template<class...> class t_vec=ublas::vector,
	class t_real = double>
t_vec<t_mat<std::complex<t_real>>> get_spin_matrices()
{
	t_vec<t_mat<std::complex<t_real>>> vec(3);
	const std::complex<t_real> i(0,1);

	vec[0] = make_mat<t_mat<std::complex<t_real>>>({{0,1}, {1,0}});
	vec[1] = make_mat<t_mat<std::complex<t_real>>>({{0,-i}, {i,0}});
	vec[2] = make_mat<t_mat<std::complex<t_real>>>({{1,0}, {0,-1}});

	return vec;
}

template<template<class...> class t_mat=ublas::matrix,
	template<class...> class t_vec=ublas::vector,
	class t_real = double>
t_vec<t_mat<std::complex<t_real>>> get_ladder_ops()
{
	t_vec<t_mat<std::complex<t_real>>> vecS = get_spin_matrices();

	t_vec<t_mat<std::complex<t_real>>> vec(2);
	const std::complex<t_real> i(0,1);

	vec[0] = vecS[0] + i*vecS[1];	// up
	vec[1] = vecS[0] - i*vecS[1];	// down

	return vec;
}


template<class t_mat=ublas::matrix<double>>
t_mat commutator(const t_mat& A, const t_mat& B)
{
	t_mat AB = ublas::prod(A, B);
	t_mat BA = ublas::prod(B, A);
	return AB - BA;
}

/**
 * spin rotation in SU(2)
 * see e.g. (Arfken 2013), p. 851
 */
template<template<class...> class t_mat = ublas::matrix, class t_real = double>
t_mat<std::complex<t_real>> rot_spin(int iComp, t_real dAngle)
{
	const auto vecS = get_spin_matrices<t_mat, ublas::vector, t_real>();
	const auto matI = unit_matrix<t_mat<std::complex<t_real>>>(2);
	const std::complex<t_real> I(0,1);

	t_mat<std::complex<t_real>> mat =
		std::complex<t_real>(std::cos(t_real(0.5)*dAngle)) * matI +
		std::complex<t_real>(std::sin(t_real(0.5)*dAngle)) * I*vecS[iComp];
	return mat;
}

}

#endif
