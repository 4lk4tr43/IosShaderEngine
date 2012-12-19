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

#include "gl/glfw.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

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
        // TODO
        // fuck it
        // why so fuckin complicated
		throw std::exception("The method or operation is not implemented.");
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

class Path
{
public:
	static string folder(string path)
	{
		size_t pos = path.find_last_of("/");
		if (pos != string::npos) return path.substr(0, pos + 1);
		throw invalid_argument(string("Couldn't determine folder of path string."));
	}

	static string file(string path)
	{
		size_t pos = path.find_last_of("/");
		if (pos != string::npos)
		{
			string fileName = path.substr(pos + 1);
			if (fileName.length() != 0)	return fileName;
		}
		throw invalid_argument(string("Couldn't determine file of path string."));
	}
};
class File
{	
	static void file_out_binary(ofstream &o, const char * data, streamsize count) 
	{		
		o.write(data, count);
		o.close();
	}
	static streamsize open_binary(streamsize count, ifstream &i, string path, ios::pos_type start) 
	{	
		if (!count) 
		{
			i.open(path, ios::in | ios::binary | ios::ate);			
			auto real_count = i.tellg();
			if (!start) i.seekg(start, ios::beg);
			return real_count;
		}
		else 
		{
			i.open(path, ios::in | ios::binary | ios::beg);
			if (!start) i.seekg(start, ios::beg);
			return count;
		}
	}

public:
	static void write(string path, const char *data, streamsize count)
	{
		ofstream o(path, ios::out | ios::binary | ios::trunc);
		file_out_binary(o, data, count);
	}
	static void append(string path, const char *data, streamsize count)
	{
		ofstream o(path, ios::out | ios::binary | ios::app);
		file_out_binary(o, data, count);
	}

	static void del(string path) 
	{

	}

	static void read(string path, char *buffer, ios::pos_type start = 0, streamsize count = 0)
	{
		ifstream i;
		i.read(buffer, open_binary(count, i, path, start));
		i.close();
	}
	static char* read_new(string path, ios::pos_type start = 0, streamsize count = 0)
	{
		ifstream i;
		auto real_count = open_binary(count, i, path, start);
		char *buffer = new char[(unsigned long)real_count];
		i.read(buffer, real_count);
		i.close();
		return buffer;
	}

	static streamsize size(string path)
	{
		ifstream i(path, ios::in | ios::binary | ios::ate);
		auto size = i.tellg();
		i.close();
		return size;
	}
};
class ObjectFile
{
public:
	void append(string path, const char *data) {}
	void push(string path, const char *data) {}	
	void insert(string path, const char *data, unsigned long index) {}

	void remove(string path, unsigned long index = 0) {}

	void get(string path, const char *data, unsigned long index = 0) {}
	char* get_new(string path, unsigned long index = 0) {}

	unsigned long count(string path) {}
};

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

void main(int argc, char **argv)
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

	make_shared<Root>()->run(loop, init, release, false);

#ifdef _DEBUG
	cin.get();
	_CrtDumpMemoryLeaks();
#endif
}