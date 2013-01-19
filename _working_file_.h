#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace glm;

#include "file.h"
#include "post_shader.h"
#include "root.h"
#include "transform.h"
#include "axis_cross.h"
#include "math_constants.h"
#include "model_importer.h"
#include "vertex_array_object.h"

Transform *models;
Shader *shader;
VertexArrayObject *vao;

void Init(Root *root)
{
// 	MeshManager mesh_manager;
// 	mesh_manager.LoadSceneFromFile("teapot.dae");
// 	Mesh mesh = mesh_manager.GetMesh(0, true, true, false, false, 0, 0);
// 	vao = mesh.ConvertToVertexArrayObjectNew();
// 	mesh_manager.UnloadScene();
// 	FILE_WRITE_SERIALIZED("test.txt", &mesh);
	Mesh m;
#ifdef WIN32
	string bundle_folder("");
#else
	string bundle_folder = root->asset_manager->base_folders->operator[](1);
#endif
	FILE_READ_SERIALIZED(bundle_folder + string("test.txt"), m, Mesh);
	vao = m.ConvertToVertexArrayObjectNew();
    OpenGL::ErrorToConsole();
	glEnable(GL_DEPTH_TEST);

	shader = root->asset_manager->GetShader("vertex_test.vert", "fragment_test.frag", 
		"position,normal", "model_view_projection_matrix,normal_matrix");
    OpenGL::ErrorToConsole();
	root->RegisterObject("Camera", new Transform());
    auto v1 = vec3(20.0f, 40.0f, 60.0f);
    auto v2 = vec3(.0f,.0f,.0f);
    auto v3 = vec3(.0f,1.0f,.0f);
	root->GetObject<Transform*>("Camera")->LookAt(v1, v2, v3);
	root->RegisterObject("Perspective", new mat4(perspective(45.0f, 4.0f/3.0f, 1.0f, 300.0f)));
	root->RegisterObject<PostShader*>("PostShader", root->asset_manager->GetPostShader("post_test.frag"));
    OpenGL::ErrorToConsole();
	models = new Transform();
	models->position = vec3(0);
	models->rotation = quat(vec3(0));

	root->RegisterObject("AxisCross", new AxisCross());
	OpenGL::ErrorToConsole();
}

void Update(Root *root)
{
    auto v1 = vec3(.0f,.01f,.0f);
	models->Rotate(v1);
}

void Render(Root *root)
{
	glClearColor(.65f,.65f,.65f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	root->GetObject<PostShader*>("PostShader")->Draw();
	glEnable(GL_DEPTH_TEST);

	auto vp = *root->GetObject<mat4*>("Perspective") * root->GetObject<Transform*>("Camera")->ViewMatrix();
	root->GetObject<AxisCross*>("AxisCross")->Draw(vp, 50);

	auto mvp = vp * models->WorldMatrix();
	shader->Activate();
	glUniformMatrix4fv(shader->UniformID(0), 1, GL_FALSE, value_ptr(mvp));
	glUniformMatrix4fv(shader->UniformID(1), 1, GL_FALSE, value_ptr(transpose(inverse(models->WorldMatrix()))));
	vao->Draw();

	glClear(GL_DEPTH_BUFFER_BIT);
	root->GetObject<AxisCross*>("AxisCross")->Draw(mvp, 10);

	if (glGetError())
		throw std::exception();
}

void Release(Root *root)
{
 	delete root->GetObject<Transform*>("Camera");
 	delete root->GetObject<mat4*>("Perspective");
 	delete root->GetObject<AxisCross*>("AxisCross");
	delete vao;
 	delete models;
}