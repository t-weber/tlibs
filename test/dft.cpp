/**
 * tlibs test file
 * @author Tobias Weber <tobias.weber@tum.de>
 * @license GPLv2 or GPLv3
 */

// gcc -DUSE_FFTW -o dft dft.cpp ../math/fourier.cpp ../math/fftw.cpp ../log/log.cpp -lstdc++ -lm -std=c++11 -lfftw3
// gcc -o dft dft.cpp ../math/fourier.cpp ../log/log.cpp -lstdc++ -lm -std=c++11

#include "../math/fourier.h"
#include <iostream>

int main()
{
	double dInR[8] = {1., 2., 3., 4., 5., 6., 7., 8.};
	double dInI[8] = {8., 7., 6., 5., 4., 3., 2., 1.};
	//double dInR[] = {123., 234.};
	//double dInI[] = {321., 432.};

	const std::size_t iSize = sizeof(dInR)/sizeof(*dInR);

	double dOutR[iSize], dOutI[8];

	tl::Fourier<double> fourier(iSize);
	fourier.fft(dInR, dInI, dOutR, dOutI);

	std::cout << "fft: ";
	for(int i=0; i<iSize; ++i)
	{
		std::cout << dOutR[i];
		if(!tl::float_equal(dOutI[i], 0.))
			std::cout << " + " << dOutI[i] << "*i";
		std::cout << ", ";
	}
	std::cout << std::endl;


	std::vector<std::complex<double>> vec = tl::arrs_to_cvec(dOutR, dOutI, iSize);
	vec = tl::dft_shift(vec, 1.);
	tl::cvec_to_arrs(vec, dOutR, dOutI);


	fourier.ifft(dOutR, dOutI, dInR, dInI);

	std::cout << "ifft: ";
	for(int i=0; i<iSize; ++i)
	{
		std::cout << dInR[i]/iSize;
		if(!tl::float_equal(dInI[i], 0., 1e-7))
			std::cout << " + " << dInI[i]/iSize << "*i";
		std::cout << ", ";
	}
	std::cout << std::endl;

	return 0;
}
