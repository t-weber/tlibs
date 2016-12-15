/**
 * random numbers
 * @author tweber
 * @date 16-aug-2013
 * @license GPLv2 or GPLv3
 */

#include "rand.h"
#include "../log/log.h"

#include <cstdlib>
#include <exception>
#include <time.h>
#include <sys/time.h>

namespace tl {

thread_local std::mt19937/*_64*/ g_randeng;
static thread_local bool g_bHasEntropy = 0;

unsigned int get_rand_seed()
{
	// seed 0: random device
	unsigned int uiSeed0 = 0;
	try
	{
		std::random_device rnd;
		g_bHasEntropy = (rnd.entropy() != 0);
		uiSeed0 = rnd();
	}
	catch(const std::exception& ex)
	{
		log_debug(ex.what());
		uiSeed0 = 0;
	}

	// seed 1: time based
	struct timeval timev;
	gettimeofday(&timev, 0);
	unsigned int uiSeed1 = timev.tv_sec ^ timev.tv_usec;

	// total seed
	unsigned int uiSeed = uiSeed0 ^ uiSeed1;
	return uiSeed;
}

void init_rand()
{
	init_rand_seed(get_rand_seed());
}

void init_rand_seed(unsigned int uiSeed)
{
	std::string strEntr;
	if(!g_bHasEntropy)
		strEntr = ", but entropy is zero";
	log_debug("Random seed: ", uiSeed, strEntr, ".");

	srand(uiSeed);
	g_randeng = std::mt19937/*_64*/(uiSeed);
}

unsigned int simple_rand(unsigned int iMax)
{
	return rand() % iMax;
}

}
