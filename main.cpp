#include <iostream>
#include <thread>
#include <string>
#include <exception>
#include <fstream>
#include <memory>
#include <chrono>
#include <random>
#include <limits>
#include <algorithm>
#include <vector>
using namespace std;

#include "glm/glm.hpp"
using namespace glm;

#ifdef WIN32
    #include "gl/glfw.h"
    #ifdef _DEBUG
        #define _CRTDBG_MAP_ALLOC
        #include <stdlib.h>
        #include <crtdbg.h>
    #endif
#endif

#include "data.h"

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
		auto loop_lambda = [&]()
		{
			if (init_loop) init_loop();
			loop(this);
			if (release_loop) release_loop();
		};

		if (seperate_thread)
		{
			thread render_thread(loop_lambda);
			render_thread.join();
		}
		else
			loop_lambda();
	}
};

/*
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

void init()
{
	if(!glfwInit()) throw exception("Could not initialize GLFW.");
	if( !glfwOpenWindow(800,600, 8,8,8,8, 16,0, GLFW_WINDOW)) throw exception("Could not create an GLFW window.");
	glfwSetWindowTitle("Test Window");
}

void release()
{
	glfwTerminate();
}

void loop(Root *root)
{
	bool running = true;
	float min_fps = numeric_limits<float>::max();

	auto startTime = chrono::system_clock::now();
	while(running && !root->star_exit)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers();

		auto nowTime = chrono::system_clock::now();
		float fps = 1000.0f / chrono::duration_cast<chrono::milliseconds>(nowTime - startTime).count();
		startTime = nowTime;
		min_fps = std::min(min_fps, fps);

		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	cout << min_fps << endl;
}
 */

int main(int argc, char **argv)
{
	Asset a;
	a.name = "this is an asset";
	a.missing_url = "";
	File::write("test.txt", (char *)&a, sizeof(a));
	string b = "this is a string";
	File::append("test.txt", (char *)&b, sizeof(b));

	auto x = File::read_new("test.txt");
	Asset *y = (Asset *)x;
	string *z = (string *)&x[sizeof(*y)];

	// make_shared<Root>()->run(loop, init, release, false);

#ifdef _DEBUG
	cin.get();
	_CrtDumpMemoryLeaks();
#endif
    return 0;
}