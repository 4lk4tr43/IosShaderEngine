#include <iostream>
#include <thread>
#include <string>
#include <exception>
#include <fstream>
#include <memory>
#include <chrono>
#include <random>
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
		throw std::exception("The method or operation is not implemented.");
		// this
		// does
	}
};

class Root
{
public:
	AssetManager *assetManager;

	void init(function<void (void)> openGLInit, string baseFolder = "")
	{
		assetManager = new AssetManager(baseFolder);

		openGLInit();
	}
	void release(function<void (void)> openGLRelease)
	{
		openGLRelease();

		delete assetManager;
	}

	void run(function<void (Root *root)> loop)
	{
		loop(this);
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

template <class T> class Random
{
	mt19937_64 engine;
	uniform_real<double> u;

public:
	Random(T lower, T upper, unsigned long seed = 0)
	{
		set_seed(seed);
		u = uniform_real<double>((double)lower, (double)upper);
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
		x = (T)u(engine);
	}
};

void initGL()
{
	if(!glfwInit()) throw exception("Could not initialize GLFW.");
	if( !glfwOpenWindow(800,600, 8,8,8,8, 16,0, GLFW_WINDOW)) throw exception("Could not create an GLFW window.");
	glfwSetWindowTitle("Test Window");
}

void releaseGL()
{
	glfwTerminate();
}

void mainLoop(Root *root)
{
	bool running = true;

	auto startTime = chrono::system_clock::now();
	while(running)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers();

		auto nowTime = chrono::system_clock::now();
		float fps = 1000.0f / chrono::duration_cast<chrono::milliseconds>(nowTime - startTime).count();
		startTime = nowTime;
		cout << fps << endl;

		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}
}

void main(int argc, char **argv)
{
	Random<int> r(-10, 10);
	
	int x = 0;

	for (int i = 0; i < 10; i++)
	{
		r >> x;
		r.set_seed(x);
		cout << x << endl;
	}

// 	auto root = new Root();
// 	root->Init(initGL);
// 	root->Simulate(mainLoop);
// 	root->Release(releaseGL);
// 	delete root;

#ifdef _DEBUG
	cin.get();
	_CrtDumpMemoryLeaks();
#endif
}