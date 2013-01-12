#ifndef mesh_manager_h__
#define mesh_manager_h__

#include <iostream>
#include <string>
using namespace std;

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "mesh.h"

class MeshManager
{
	Assimp::Importer _importer;
	aiScene *_scene;

public:
	class MeshManagerLoadException : exception {};

	MeshManager()
	{
		_scene = nullptr;
	}

	~MeshManager()
	{
		UnloadScene();
	}

	void LoadSceneFromFile(string file_path, 
		unsigned int post_process_steps = 0)
	{
		UnloadScene();
		_scene = (aiScene *)_importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_ImproveCacheLocality | 
			aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | post_process_steps);
		if (!_scene)
		{
			throw MeshManagerLoadException();
			cout << _importer.GetErrorString() << endl;
		}
	}

	void LoadSceneFromMemory(void *data, size_t data_size, 
		unsigned int post_process_steps = 0)
	{
		UnloadScene();
		_scene = (aiScene *)_importer.ReadFileFromMemory(data, data_size, aiProcess_Triangulate | aiProcess_ImproveCacheLocality |
			aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes| post_process_steps);
		if (!_scene)
		{
			throw MeshManagerLoadException();
			cout << _importer.GetErrorString() << endl;
		}
	}

	void UnloadScene()
	{
		if (!_scene)
			return;
		_importer.FreeScene();
		_scene = nullptr;
	}

	Mesh GetMesh(unsigned int mesh_index = 0, 
		bool use_position = true, bool use_normal = true, bool use_tangent = true, bool use_bitangent = true,
		unsigned int use_uv_up_to_channel = 8, unsigned int use_color_up_to_channel = 8)
	{
		auto mesh = _scene->mMeshes[mesh_index];
		Mesh mesh_data;

		mesh_data.primitive_type = GL_TRIANGLES;
		mesh_data.vertex_count = mesh->mNumVertices;

		if (use_position)
		{
			mesh_data.vertex_data.push_back((void*)mesh->mVertices);
			mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::POSITION);
			mesh_data.vertex_description.AddAttribute(3, GL_FLOAT, GL_FALSE);
		}
		if (use_normal && mesh->HasNormals)
		{
			mesh_data.vertex_data.push_back((void*)mesh->mNormals);
			mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::NORMAL);
			mesh_data.vertex_description.AddAttribute(3, GL_FLOAT, GL_FALSE);
		}
		if (use_tangent && mesh->HasTangentsAndBitangents)
		{
			mesh_data.vertex_data.push_back((void*)mesh->mTangents);
			mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::TANGENT);
			mesh_data.vertex_description.AddAttribute(3, GL_FLOAT, GL_FALSE);
		}
		if (use_bitangent && mesh->HasTangentsAndBitangents)
		{
			mesh_data.vertex_data.push_back((void*)mesh->mBitangents);
			mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::BITANGENT);
			mesh_data.vertex_description.AddAttribute(3, GL_FLOAT, GL_FALSE);
		}
		if (mesh->HasTextureCoords)
		{
			auto uv_channels_to_load = (use_uv_up_to_channel < mesh->GetNumUVChannels()) ?  use_uv_up_to_channel : mesh->GetNumUVChannels();
			for (int j = 0; j < uv_channels_to_load; ++j)
			{
				mesh_data.vertex_data.push_back((void*)mesh->mTextureCoords[j]);
				mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::UV);
				mesh_data.vertex_description.AddAttribute(2, GL_FLOAT, GL_FALSE);
			}
		}
		if (mesh->HasVertexColors)
		{
			auto color_channels_to_load = (use_color_up_to_channel < mesh->GetNumColorChannels()) ?  use_color_up_to_channel : mesh->GetNumColorChannels();
			for (int j = 0; j < color_channels_to_load; ++j)
			{
				mesh_data.vertex_data.push_back((void*)mesh->mColors[j]);
				mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::COLOR);
				mesh_data.vertex_description.AddAttribute(4, GL_FLOAT, GL_FALSE);
			}
		}

		// TODO create and add bone id/weight attribute

		unsigned int max_index = 0;
		for (unsigned int i = 0 ; i < mesh->mNumFaces ; ++i) 
		{
			const aiFace &face = mesh->mFaces[i];				
			mesh_data.indices.push_back(face.mIndices[0]);
			mesh_data.indices.push_back(face.mIndices[1]);
			mesh_data.indices.push_back(face.mIndices[2]);
			max_index = std::max(std::max(max_index, face.mIndices[0]), std::max(face.mIndices[1], face.mIndices[2]));
		}

		mesh_data.max_index = max_index;
		return mesh_data;
	}

	vector<Mesh> GetMeshes(bool use_position = true, bool use_normal = true, bool use_tangent = true, bool use_bitangent = true,
		unsigned int use_uv_up_to_channel = 8, unsigned int use_color_up_to_channel = 8)
	{
		vector<Mesh> result;
		for (unsigned int i = 0; i < _scene->mNumMeshes; ++i)
			result.push_back(GetMesh(i, use_position, use_normal, use_tangent, use_bitangent, use_uv_up_to_channel, use_color_up_to_channel));
		return result;
	}
};

#endif
