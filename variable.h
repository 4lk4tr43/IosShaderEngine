#ifndef variable_h__
#define variable_h__

#include <random>
using namespace std;

template <class T> class RandomReal
{
	mt19937 engine;
	uniform_real_distribution<T> u;

public:
	RandomReal(T lower, T upper, unsigned long seed = 0)
	{
		set_seed(seed);
		u = uniform_real_distribution<T>(lower, upper);
	}
    
    void operator>>(T &x) {	x = u(engine); }
    
	void set_seed(unsigned long seed) 
	{
		if (seed)
			engine.seed(seed);
		else
			engine.seed((unsigned long)time(nullptr));
	}
};

template <class T> class RandomInteger
{
	mt19937 engine;
	uniform_int_distribution<T> u;

public:
	RandomInteger(T lower, T upper, unsigned long seed = 0)
	{
		set_seed(seed);
		u = uniform_int_distribution<T>(lower, upper);
	}
    
    void operator>>(T &x) {	x = u(engine); }
    
	void set_seed(unsigned long seed) 
	{
		if (seed)
			engine.seed(seed);
		else
			engine.seed((unsigned long)time(nullptr));
	}
};

#endif
