
#include "data.h"
#include "variable.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <thread>
using namespace std;

#include "glm/glm.hpp"
using namespace glm;

class AssetManager
{
public:
	string baseFolder;

	AssetManager(string baseFolder = "")
	{
		this->baseFolder = baseFolder;
	}

	void load_screen(string screenName)
	{
		throw std::exception();
	}
};

class Asset
{
public:
	enum Type
	{
		unknown,
		data,

		texture,
		shader,
		mesh,
		skeleton,

		audio,
		music,

		gui,
		screen
	};

	Asset()
	{
		type = Type::unknown;
		priority = 0;
	}

	Type type;
	unsigned char priority;
	string missing_url;
	string name;
};

class Root
{
public:
	AssetManager *assetManager;
	bool seperate_thread, star_exit;

	Root(string baseFolder = "")
	{
		assetManager = new AssetManager(baseFolder);
		star_exit = false;
	}
	~Root()
	{
		delete assetManager;
	}

	void run(function<void (Root *root)> loop, function<void (void)> init_loop = nullptr, function<void (void)> release_loop = nullptr, bool seperate_thread = true)
	{		
		auto aggregation_lambda = [&]()
		{
			if (init_loop) init_loop();
			if (loop) loop(this);
			if (release_loop) release_loop();
		};

		if (seperate_thread)
		{
			thread running_thread(aggregation_lambda);
			running_thread.join();
		}
		else
			aggregation_lambda();
	}
};

#include <typeindex>

void test(char *param)
{
    cout << "Starting test..." << endl;
    
    string path = *(string*)param;
    File::del(path);
    
    float v[3];
    v[0] = 1.0f;
    v[1] = 2.0f;
    v[2] = 3.0f;
    float w[3];
    w[0] = 4.0f;
    w[1] = 5.0f;
    w[2] = 6.0f;
    float u[3];
    u[0] = 7.0f;
    u[1] = 8.0f;
    u[2] = 9.0f;
    
    ObjectFile::append(path, (const char*)v, typeid(float), sizeof(float) * 3);
    ObjectFile::append(path, (const char*)w, typeid(w), sizeof(float) * 3);
    ObjectFile::push(path, (const char*)u, typeid(float), sizeof(float) * 3);
    ObjectFile::remove(path, 1);
    
    auto ti = ObjectFile::type_index_new();
    auto r = (float*)ObjectFile::get_new(path, 1, ti);
    
    cout << (*ti == typeid(float[3])) << endl;
    
    for (int i = 0; i < 3; ++i)
    {
        cout << r[i] << " ";
    }
    cout << endl << ObjectFile::count(path) << endl;
    
    delete[] ti;
    delete[] r;
    File::del(path);
    
    cout << "...end of test." << endl;
}
