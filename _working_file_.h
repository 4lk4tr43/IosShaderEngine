
#include "file.h"
#include "post_shader.h"
#include "root.h"

static PostShader *ps;

void TestInit(Root *root)
{
	glViewport(0, 0, 800, 600);
    
    string error_log;
    
    string* shader_string = root->asset_manager->Get<string*>("fragment_test.frag", "", true);
    cout << *shader_string << endl;
	ps = new PostShader((GLchar*)shader_string->c_str(), nullptr, &error_log);
    //root->asset_manager->Unload("fragment_test.frag");
    cout << error_log << endl;
}

void TestRender(Root *root)
{
	ps->Draw();
}

void TestRelease(Root *root)
{
	delete ps;
}