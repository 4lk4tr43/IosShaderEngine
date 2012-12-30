#ifndef timer_h__
#define timer_h__

#include <chrono>
using namespace std;

template <class T> class Timer
{
    chrono::time_point<chrono::high_resolution_clock> _start_time;
    
public:
    unsigned long Reset()
    {
        auto time_since_start = Stop();
        Start();
        return time_since_start;
    }
    
    void Start()
    {
        _start_time = chrono::high_resolution_clock::now();
    }
    
    unsigned long Stop()
    {
        auto time_now = chrono::high_resolution_clock::now();
        auto elapsed_time = time_now - _start_time;
        
        return chrono::duration_cast<T>(elapsed_time).count();
    }
};

#endif
