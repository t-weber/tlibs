/**
 * advanced linalg helpers
 *
 * @author: tweber
 * @date: 30-apr-2013
 * @license GPLv2 or GPLv3
 */

#ifndef __TLIBS_LINALG2__
#define __TLIBS_LINALG2__


#include "math.h"
#include "linalg.h"
#include <complex>


namespace tl {

#if !defined NO_LAPACK && !defined USE_LAPACK
	#define USE_LAPACK
#endif


#ifdef NO_LAPACK

template<typename T/*=double*/>
bool qr(const ublas::matrix<T>& M, ublas::matrix<T>& Q, ublas::matrix<T>& R)
{
	return qr_decomp(M, Q, R);
}

template<typename T=double>
bool eigenvec_sym(const ublas::matrix<T>& mat,
	std::vector<ublas::vector<T>>& evecs, std::vector<T>& evals)
{
	return eigenvec_sym_simple(mat, evecs, evals);
}

template<typename T=double>
bool eigenvec_approxsym(const ublas::matrix<T>& mat,
	std::vector<ublas::vector<T>>& evecs, std::vector<T>& evals)
{
	return eigenvec_approxsym_simple(mat, evecs, evals);
}

#else

template<typename T/*=double*/>
bool qr(const ublas::matrix<T>& M, ublas::matrix<T>& Q, ublas::matrix<T>& R);


template<typename T=double>
bool eigenvec(const ublas::matrix<T>& mat,
	std::vector<ublas::vector<T> >& evecs_real, std::vector<ublas::vector<T>>& evecs_imag,
	std::vector<T>& evals_real, std::vector<T>& evals_imag);

template<typename T=double>
bool eigenvec_cplx(const ublas::matrix<std::complex<T>>& mat,
	std::vector<ublas::vector<std::complex<T>> >& evecs,
	std::vector<std::complex<T>>& evals);


template<typename T=double>
bool eigenvec_sym(const ublas::matrix<T>& mat,
	std::vector<ublas::vector<T>>& evecs, std::vector<T>& evals);

template<typename T=double>
bool eigenvec_herm(const ublas::matrix<std::complex<T>>& mat,
	std::vector<ublas::vector<std::complex<T>>>& evecs,
	std::vector<T>& evals);


template<typename T=double>
bool singvec(const ublas::matrix<T>& mat,
	ublas::matrix<T>& matU, ublas::matrix<T>& matV, std::vector<T>& vecsvals);

template<typename T=double>
bool singvec_cplx(const ublas::matrix<std::complex<T>>& mat,
	ublas::matrix<std::complex<T>>& matU, ublas::matrix<std::complex<T>>& matV,
	std::vector<T>& vecsvals);


template<typename T=double>
bool eigenvec_approxsym(const ublas::matrix<T>& mat,
	std::vector<ublas::vector<T>>& evecs, std::vector<T>& evals)
{
	ublas::matrix<T> matU, matV;
	bool bOk = singvec(mat, matU, matV, evals);

	evecs.resize(matV.size2());
	for(std::size_t j=0; j<matV.size2(); ++j)
		evecs[j] = get_column(matV, j);

	return bOk;
}



#ifdef TLIBS_INC_HDR_IMPLS
}
#include "linalg2_impl.h"
namespace tl {
#endif


#endif

}

#endif
