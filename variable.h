#ifndef variable_h__
#define variable_h__

#include <deque>
#include <random>
#include <sstream>
#include <string>
using namespace std;

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

class StringTokenizer
{
    deque<string> _lines;
    
public:
    StringTokenizer(string stringToSplit, string seperatorString = "\n")
    {
        auto stringToSplitSize = stringToSplit.size();
        auto cstring = new char[stringToSplitSize + 1];
        memcpy(cstring, (const void*)stringToSplit.c_str(), stringToSplitSize * sizeof(char));
        cstring[stringToSplitSize] = '\0';
        auto cseperator = seperatorString.c_str();
        auto token = strtok(cstring, cseperator);
        while (token)
        {
            _lines.push_back(string(token));
            token = strtok(0, cseperator);
        }
        delete[] cstring;
    }
    
    size_t line_count()
    {
        return _lines.size();
    }
    
    string operator[](unsigned long index)
    {
        return _lines[index];
    }
};

#endif
