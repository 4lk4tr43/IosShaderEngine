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

#include "tree_node.h"

Transform *models;
Shader *shader;
VertexArrayObject *vao;

void Init(Root *root)
{
#ifdef WIN32
    string bundle_folder("");
    
    ModelImporter mesh_manager;
	mesh_manager.LoadSceneFromFile("TubeSkeletal.dae");
 	Mesh mesh = mesh_manager.GetMesh(0, 4, true, true, false, false, 0, 0);
	
	auto skeleton = mesh_manager.GetMeshSkeletonNew(0);

 	mesh_manager.UnloadScene();
	
 	FILE_WRITE_SERIALIZED("test.mesh", &mesh);
	FILE_WRITE_SERIALIZED("test.skeleton", skeleton)
	delete skeleton;

#else
	string bundle_folder = root->asset_manager->base_folders->operator[](1);
#endif
    
	Mesh m;    
	FILE_READ_SERIALIZED(bundle_folder + string("test.mesh"), m, Mesh);
	vao = m.ConvertToVertexArrayObjectNew();

 	Skeleton *s;
 	FILE_READ_SERIALIZED_NEW(bundle_folder + string("test.skeleton"), s, Skeleton);
 	root->RegisterObject<Skeleton*>("Skeleton", s);

	auto abst = s->GetAbsoluteTransformations();
	auto absm = new vector<mat4>();
	for (auto iter = abst.begin(); iter != abst.end(); iter++)
		absm->push_back(mat4()/*(*iter).WorldMatrix()*/);
	root->RegisterObject<vector<mat4>*>("Pose", absm);

    OpenGL::ErrorToConsole();
	glEnable(GL_DEPTH_TEST);

	shader = root->asset_manager->GetShader("vertex_bone_test.vert", "fragment_test.frag", 
		"position,normal,bone_ids,bone_weights", "model_view_projection_matrix,normal_matrix,bones");
    OpenGL::ErrorToConsole();
	root->RegisterObject("Camera", new Transform());
    auto v1 = vec3(60.0f, 100.0f, 140.0f);
    auto v2 = vec3(.0f,.0f,.0f);
    auto v3 = vec3(.0f,1.0f,.0f);
	root->GetObject<Transform*>("Camera")->LookAt(v1, v2, v3);
	root->RegisterObject<mat4*>("Perspective", new mat4(perspective(45.0f, 4.0f/3.0f, 1.0f, 300.0f)));
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
	auto pose = root->GetObject<vector<mat4>*>("Pose");
	//pose->operator[](0) = mat4(0);	

	shader->Activate();
	glUniformMatrix4fv(shader->UniformID(0), 1, GL_FALSE, value_ptr(mvp));
	glUniformMatrix4fv(shader->UniformID(1), 1, GL_FALSE, value_ptr(transpose(inverse(models->WorldMatrix()))));
	glUniformMatrix4fv(shader->UniformID(2), 1, GL_FALSE, value_ptr(pose->operator[](0)));
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
	delete root->GetObject<Skeleton*>("Skeleton");
}