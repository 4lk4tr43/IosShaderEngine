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

#include "opengl.h"

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
        
    cout << "...end of test." << endl;
}
