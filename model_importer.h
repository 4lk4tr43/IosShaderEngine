#ifndef model_impoerter_h__
#define model_impoerter_h__

#include <iostream>
#include <string>
using namespace std;

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "mesh.h"

class ModelImporter
{
	Assimp::Importer _importer;
	aiScene *_scene;

	template <class T>
	vector<T> CopyMemoryToVector(T* memory, size_t element_count)
	{
		vector<T> result;
		for (size_t i = 0; i < element_count; ++i)
			result.push_back(memory[i]);
		return result;
	}

public:
	class MeshManagerLoadException : exception {};

	ModelImporter()
	{
		_scene = nullptr;
	}

	~ModelImporter()
	{
		UnloadScene();
	}

	void LoadSceneFromFile(string file_path, 
		unsigned int post_process_steps = 0)
	{
		UnloadScene();
		_scene = (aiScene *)_importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_ImproveCacheLocality | 
			aiProcess_JoinIdenticalVertices | post_process_steps);
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
			aiProcess_JoinIdenticalVertices | post_process_steps);
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

		if (use_position && mesh->HasPositions())
		{
			mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::POSITION);
			VertexAttribute attribute(3, GL_FLOAT, GL_FALSE);
			mesh_data.vertex_description += attribute;
			mesh_data.vertex_data.push_back(CopyMemoryToVector<char>((char*)mesh->mVertices, attribute.Size() * mesh_data.vertex_count));
		}
		if (use_normal && mesh->HasNormals())
		{
			mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::NORMAL);
			VertexAttribute attribute(3, GL_FLOAT, GL_FALSE);
			mesh_data.vertex_description += attribute;
			mesh_data.vertex_data.push_back(CopyMemoryToVector<char>((char*)mesh->mNormals, attribute.Size() * mesh_data.vertex_count));
		}
		if (use_tangent && mesh->HasTangentsAndBitangents())
		{
			mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::TANGENT);
			VertexAttribute attribute(3, GL_FLOAT, GL_FALSE);
			mesh_data.vertex_description += attribute;
			mesh_data.vertex_data.push_back(CopyMemoryToVector<char>((char*)mesh->mTangents, attribute.Size() * mesh_data.vertex_count));
		}
		if (use_bitangent && mesh->HasTangentsAndBitangents())
		{
			mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::BITANGENT);
			VertexAttribute attribute(3, GL_FLOAT, GL_FALSE);
			mesh_data.vertex_description += attribute;
			mesh_data.vertex_data.push_back(CopyMemoryToVector<char>((char*)mesh->mBitangents, attribute.Size() * mesh_data.vertex_count));
		}
		if (use_uv_up_to_channel)
		{
			auto uv_channels_to_load = (use_uv_up_to_channel < mesh->GetNumUVChannels()) ?  use_uv_up_to_channel : mesh->GetNumUVChannels();
			for (unsigned int j = 0; j < uv_channels_to_load; ++j)
			{
				if (!mesh->HasTextureCoords(j))
					continue;
				mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::UV);
				VertexAttribute attribute(2, GL_FLOAT, GL_FALSE);
				mesh_data.vertex_description += attribute;
				mesh_data.vertex_data.push_back(CopyMemoryToVector<char>((char*)mesh->mTextureCoords[j], attribute.Size() * mesh_data.vertex_count));
			}
		}
		if (use_color_up_to_channel)
		{
			auto color_channels_to_load = (use_color_up_to_channel < mesh->GetNumColorChannels()) ?  use_color_up_to_channel : mesh->GetNumColorChannels();
			for (unsigned int j = 0; j < color_channels_to_load; ++j)
			{
				if (!mesh->HasVertexColors(j))
					continue;
				mesh_data.mesh_attributes.push_back(Mesh::MeshAttributeType::COLOR);
				VertexAttribute attribute(4, GL_FLOAT, GL_FALSE);
				mesh_data.vertex_description += attribute;
				mesh_data.vertex_data.push_back(CopyMemoryToVector<char>((char*)mesh->mColors[j], attribute.Size() * mesh_data.vertex_count));
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
