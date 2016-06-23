/*
 * clang -I/usr/include/lapacke -o spin ../test/spin.cpp ../log/log.cpp -lstdc++ -std=c++11 -lm -llapacke -llapack
 * @author tweber
 */

#define TLIBS_USE_GLOBAL_OPS
#include "../math/linalg.h"
#include "../math/linalg_ops.h"
#include "../math/linalg2.h"
#include "../math/linalg2_impl.h"

#include <iostream>

using namespace tl;
using t_mat = ublas::matrix<std::complex<double>>;
using t_vec = ublas::vector<std::complex<double>>;

int main()
{
	auto vec = get_spin_matrices();
	auto I = unit_matrix<t_mat>(2);

	// operator components for spin 1
	t_mat S1x = tensor_prod(vec[0], I);
	t_mat S1y = tensor_prod(vec[1], I);
	t_mat S1z = tensor_prod(vec[2], I);

	// operator components for spin 2
	t_mat S2x = tensor_prod(I, vec[0]);
	t_mat S2y = tensor_prod(I, vec[1]);
	t_mat S2z = tensor_prod(I, vec[2]);

	// two-spin operator
	t_mat S1S2 = S1x*S2x + S1y*S2y + S1z*S2z;

	std::cout << "S1*S2 (direct) = " << S1S2 << std::endl;


	std::vector<t_vec> evecs;
	std::vector<std::complex<double>> evals;
	if(!eigenvec_cplx(S1S2, evecs, evals))
		std::cerr << "Cannot calculate eigenvectors." << std::endl;

	for(std::size_t iEV=0; iEV<evecs.size(); ++iEV)
	{
		std::cout << "evec: " << evecs[iEV]
			<< ", eval: " << evals[iEV]
			<< std::endl;
	}


	// ---------------------------------------------------------


	auto vecLadder = get_ladder_ops();
	//std::cout << vecLadder << std::endl;

	t_mat C = 0.5 * (tensor_prod(vecLadder[0], I) * tensor_prod(I, vecLadder[1]) +
		tensor_prod(vecLadder[1], I) * tensor_prod(I, vecLadder[0]))
		+ S1z*S2z;
	std::cout << "S1*S2 (via ladder) = " << C << std::endl;
	//std::cout << int(tl::get_linalg_type<decltype(C)>::value) << std::endl;
	//std::cout << int(tl::get_linalg_type<decltype(S1S2)>::value) << std::endl;
	std::cout << std::boolalpha << (C == S1S2) << std::endl;
	return 0;
}
