#include "random.h"

std::uniform_real_distribution<double> Random::_distribution(0.0, 1.0);
std::default_random_engine Random::_engine;
int Random::_seed;

double Random::next() { return _distribution(_engine); }

double Random::next(double from, double to) 
{ 
	auto n = next();
	n = (n * (to - from)) + from;

	return n;
}

int Random::nextInt(int from, int to)
{
	return (int)next((double)from, (double)to);
}

void Random::setSeed()
{
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
	(
		std::chrono::system_clock::now().time_since_epoch()
	);

	setSeed((int)ms.count());
}

void Random::setSeed(int seed)
{ 
	_seed = seed; 
	_engine.seed(_seed);
}