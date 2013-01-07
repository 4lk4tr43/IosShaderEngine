#include "file.h"
#include "post_shader.h"
#include "root.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace glm;

#include "transform_node.h"

TransformNode *camera;
mat4 projection;
TransformNode *world = new TransformNode();

void Init(Root *root)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( "teapot.dae",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if (scene)
		cout << "Scene loaded" << endl;

	auto vao = new VertexArrayObject();
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
	root->RegisterObject("Vao", vao);
	root->RegisterObject<Shader*>("Shader", root->asset_manager->GetShader("vertex_test.vert", "fragment_test.frag", "position,normal", "wvp"));
	root->RegisterObject<PostShader*>("PostShader", root->asset_manager->GetPostShader("post_test.frag"));

	glEnable(GL_DEPTH_TEST);
		OpenGL::ErrorToConsole();

	camera = new TransformNode();
	camera->LookAt(vec3(.0f,.0f,-150.0f), vec3(.0f,.0f,.0f), vec3(.0f,1.0f,.0f));

	projection = perspective(45.0f, 4.0f/3.0f, 1.0f, 300.0f);

	world = new TransformNode();
	world->position = vec3(3);
	world->rotation = quat(vec3(0.5, 0 , 0));
}

void Update(Root *root)
{
	world->Rotate(vec3(.0005f,.0f,.0f));
 	camera->Rotate(vec3(.0f,.001f,0));
}

void Render(Root *root)
{
	glClearColor(.65f,.65f,.65f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glDisable(GL_DEPTH_TEST);
	root->GetObject<PostShader*>("PostShader")->Draw();
	glEnable(GL_DEPTH_TEST);
	auto shader = root->GetObject<Shader*>("Shader");
	shader->Activate();
	auto vp = projection * camera->ViewMatrix();
	auto wvp = vp * world->WorldMatrix();
	glUniformMatrix4fv(shader->UniformID(0), 1, 0, value_ptr(wvp));
	root->GetObject<VertexArrayObject*>("Vao")->Draw();
}

void Release(Root *root)
{
	delete root->GetObject<VertexArrayObject*>("Vao");
	delete camera;
}