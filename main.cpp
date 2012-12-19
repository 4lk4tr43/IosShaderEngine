#include "_working_file_.h"

#ifdef WIN32
    #include "gl/glfw.h"
    #ifdef _DEBUG
        #define _CRTDBG_MAP_ALLOC
        #include <stdlib.h>
        #include <crtdbg.h>
    #endif
#endif

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

int main(int argc, char *argv[])
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

    return 0;
}