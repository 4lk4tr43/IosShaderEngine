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

#include "file.h"
#include "post_shader.h"

static PostShader *ps;

void TestInit()
{
	glViewport(0, 0, 800, 600);

	ifstream input_stream("Shader/fragment_test.fsh");
	string shader_string;
	shader_string.assign(istreambuf_iterator<char>(input_stream), istreambuf_iterator<char>());
	string error_log;
	input_stream.close();
	ps = new PostShader((GLchar*)shader_string.c_str(), nullptr, &error_log);
	cout << error_log << endl << shader_string << endl;
}

void Test(char *param)
{
	ps->Draw();
}

void TestRelease()
{
	delete ps;
}