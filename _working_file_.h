
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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

Transform *models;
Shader *shader;
VertexArrayObject *vao;

void Init(Root *root)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( "teapot.dae",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if (scene)
		cout << "Scene loaded" << endl;

	vao = new VertexArrayObject();
	vector<GLvoid*> v;
	v.push_back((GLvoid*)scene->mMeshes[0]->mVertices);
	v.push_back((GLvoid*)scene->mMeshes[0]->mNormals);

	vector<GLuint> indices;

	for (unsigned int i = 0 ; i < scene->mMeshes[0]->mNumFaces ; ++i) 
	{
		const aiFace& Face = scene->mMeshes[0]->mFaces[i];
		indices.push_back(Face.mIndices[0]);
		indices.push_back(Face.mIndices[1]);
		indices.push_back(Face.mIndices[2]);
	}

	vao->AddVertices(GL_STATIC_DRAW, GL_TRIANGLES, VertexDescription::PositionNormal(), v, scene->mMeshes[0]->mNumVertices);
	vao->AddIndices(GL_STATIC_DRAW, GL_UNSIGNED_INT, (GLvoid*)&indices[0], indices.size());

	glEnable(GL_DEPTH_TEST);

	shader = root->asset_manager->GetShader("vertex_test.vert", "fragment_test.frag", 
		"position,normal", "model_view_projection_matrix,normal_matrix");

	root->RegisterObject("Camera", new Transform());
	root->GetObject<Transform*>("Camera")->LookAt(vec3(20.0f, 40.0f, 60.0f), vec3(.0f,.0f,.0f), vec3(.0f,1.0f,.0f));
	root->RegisterObject("Perspective", new mat4(perspective(45.0f, 4.0f/3.0f, 1.0f, 300.0f)));
	root->RegisterObject<PostShader*>("PostShader", root->asset_manager->GetPostShader("post_test.frag"));

	models = new Transform();
	models->position = vec3(0);
	models->rotation = quat(vec3(0));

	root->RegisterObject("AxisCross", new AxisCross());
	OpenGL::ErrorToConsole();
}

void Update(Root *root)
{
	models->Rotate(vec3(.0f,.01f,.0f));
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