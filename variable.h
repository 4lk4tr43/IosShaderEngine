#ifndef variable_h__
#define variable_h__

#include <random>
using namespace std;

template <class T> class RandomReal
{
	mt19937_64 engine;
	uniform_real<T> u;

public:
	RandomReal(T lower, T upper, unsigned long seed = 0)
	{
		set_seed(seed);
		u = uniform_real<T>(lower, upper);
	}
	void set_seed(unsigned long seed) 
	{
		if (seed)
			engine.seed(seed);
		else
			engine.seed((unsigned long)time(nullptr));
	}
	void operator>>(T &x)
	{
		x = u(engine);
	}
};

template <class T> class RandomInteger
{
	mt19937_64 engine;
	uniform_int<T> u;

public:
	RandomInteger(T lower, T upper, unsigned long seed = 0)
	{
		set_seed(seed);
		u = uniform_int<T>(lower, upper);
	}
	void set_seed(unsigned long seed) 
	{
		if (seed)
			engine.seed(seed);
		else
			engine.seed((unsigned long)time(nullptr));
	}
	void operator>>(T &x)
	{
		x = u(engine);
	}
};

#endif
