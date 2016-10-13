// gcc -o rand rand.cpp ../math/rand.cpp ../log/log.cpp -lstdc++ -lm -lpthread -std=c++11

#include "../math/rand.h"
#include <iostream>

int main()
{
	tl::init_rand();

	std::cout << tl::rand01<float>() << std::endl;
	std::cout << tl::rand_minmax<float>(0., 10.) << std::endl;
	std::cout << tl::rand_minmax<int>(0, 10) << std::endl;
	std::cout << tl::rand_binomial<int, float>(100, 0.5) << std::endl;

	auto vecRnd = tl::rand_norm_nd<>({1., 2., 3.}, {0.25, 0.5, 0.75});
	for(auto d : vecRnd)
		std::cout << d << ", ";
	std::cout << std::endl;

	auto vecRnd2 = tl::rand_exp_nd<>({1., 2., 3.});
	for(auto d : vecRnd2)
		std::cout << d << ", ";
	std::cout << std::endl;

	return 0;
}
