#include "_working_file_.h"

#ifdef WIN32
	#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#endif
#endif

int main(int argc, char *argv[])
{
	if(!glfwInit())
		throw exception("Could not initialize GLFW.");
	if( !glfwOpenWindow(800,600, 8,8,8,8, 16,0, GLFW_WINDOW)) 
		throw exception("Could not create an GLFW window.");
	glfwSetWindowTitle("Test Window");
	if (GLEW_OK != glewInit())
		throw exception("Could not initialize an GLEW.");

	auto root = new Root("C:/Users/4lk4t_000/Documents/Visual Studio 2012/Projects/ConsoleApplication_Test/");
	Init(root);
	while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED))
	{
		Update(root);
		Render(root);
		glfwSwapBuffers();
	}
	Release(root);
	glfwTerminate();
	delete root;

#ifdef _DEBUG
	cin.get();
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}