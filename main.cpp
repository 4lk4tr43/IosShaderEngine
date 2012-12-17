#include <iostream>
#include <thread>
#include <string>
#include <exception>
#include <chrono>
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

	void LoadScreen(string screenName)
	{
		throw std::exception("The method or operation is not implemented.");
	}
};

class Root
{
public:
	AssetManager *assetManager;

	void Init(function<void (void)> openGLInit, string baseFolder = "")
	{
		assetManager = new AssetManager(baseFolder);

		openGLInit();
	}
	void Release(function<void (void)> openGLRelease)
	{
		openGLRelease();

		delete assetManager;
	}

	void Simulate(function<void (void)> loop)
	{
		loop();
	}
};

static class Path
{
public:
	static string getFolder(string path)
	{
		size_t pos = path.find_last_of("/");
		if (pos != string::npos) return path.substr(0, pos + 1);
		throw invalid_argument(string("Couldn't determine folder of path string."));
	}

	static string getFile(string path)
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

void mainLoop()
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
	auto root = new Root();
	root->Init(initGL);
	root->Simulate(mainLoop);
	root->Release(releaseGL);
	delete root;

#ifdef _DEBUG
	cin.get();
	_CrtDumpMemoryLeaks();
#endif
}