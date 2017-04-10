/**
 * tlibs test file
 * @author Tobias Weber <tobias.weber@tum.de>
 * @license GPLv2 or GPLv3
 */

// gcc -o rand rand.cpp ../math/rand.cpp ../log/log.cpp -lstdc++ -lm -lpthread -std=c++11

#include "../math/rand.h"
#include <iostream>
#include <thread>

std::mutex mtx;

void rnd_fkt(unsigned iSeed)
{
	mtx.lock();
	tl::init_rand_seed(iSeed);

	std::cout << "seed: " << iSeed << std::endl;
	std::cout << tl::rand01<float>() << " " << tl::rand01<float>() << std::endl;
	std::cout << tl::rand_minmax<float>(0., 10.) << std::endl;
	std::cout << tl::rand_minmax<int>(0, 10) << std::endl;
	std::cout << tl::rand_binomial<int, float>(100, 0.5) << std::endl;

	auto vecRnd = tl::rand_norm_nd<>({1., 2., 3.}, {0.25, 0.5, 0.75});
	for(auto d : vecRnd)
		std::cout << d << ", ";
	std::cout << std::endl;

	vecRnd = tl::rand_norm_nd<>({1., 2., 3.}, {0.25, 0.5, 0.75});
	for(auto d : vecRnd)
		std::cout << d << ", ";
	std::cout << std::endl;

	auto vecRnd2 = tl::rand_exp_nd<>({1., 2., 3.});
	for(auto d : vecRnd2)
		std::cout << d << ", ";
	std::cout << std::endl;

	mtx.unlock();
}

int main()
{
	std::thread th1([]{ rnd_fkt(1); });
	std::thread th2([]{ rnd_fkt(2); });

	th1.join();
	th2.join();
	return 0;
}
