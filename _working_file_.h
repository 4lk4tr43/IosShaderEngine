
#include "file.h"
#include "post_shader.h"
#include "root.h"

void TestInit(Root *root)
{
	root->RegisterObject<PostShader*>("PostShader", root->asset_manager->GetPostShader("fragment_test.frag"));
}

void TestRender(Root *root)
{
	root->GetObject<PostShader*>("PostShader")->Draw();
}

void TestRelease(Root *root)
{

}