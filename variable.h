#ifndef variable_h__
#define variable_h__

#include <random>
using namespace std;

class RandomParent
{
protected:
    void *_engine;
    bool _bit64;
    
    RandomParent(unsigned long seed = 0, bool bit64 = false)
    {
        _bit64 = bit64;
        
        if (_bit64)
            _engine = new mt19937_64();
        else
            _engine = new mt19937();
        
        set_seed(seed);
    }
    ~RandomParent()
    {
        if (_bit64)
            delete ((mt19937_64 *)_engine);
        else
            delete ((mt19937 *)_engine);
    }
    
public:
    void set_seed(unsigned long seed)
	{
		if (seed)
            if (_bit64)
                ((mt19937_64 *)_engine)->seed(seed);
            else
                ((mt19937 *)_engine)->seed(seed);
        else
            if (_bit64)
                ((mt19937_64 *)_engine)->seed((unsigned long)time(nullptr));
            else
                ((mt19937 *)_engine)->seed((unsigned long)time(nullptr));
	}
};

template <class T> class RandomReal : public RandomParent
{
	uniform_real_distribution<T> _u;
    
public:
	RandomReal(T lower, T upper, unsigned long seed = 0, bool bit64 = false) : RandomParent(seed, bit64)
	{
		_u = uniform_real_distribution<T>(lower, upper);
	}
    
	void operator>>(T &x)
	{
		x = _u(*(this->_engine));
	}
};

template <class T> class RandomInteger : public RandomParent
{
	uniform_int_distribution<T> u;

public:
	RandomInteger(T lower, T upper, unsigned long seed = 0, bool bit64 = false) : RandomParent(seed, bit64)
	{        
		u = uniform_int_distribution<T>(lower, upper);
	}
    
	void operator>>(T &x)
	{
		x = _u(*(this->_engine));
	}
};

#endif
