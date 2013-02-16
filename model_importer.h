#ifndef model_impoerter_h__
#define model_impoerter_h__

#include <iostream>
#include <string>
using namespace std;

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "mesh.h"
#include "skeleton.h"

#define MODEL_IMPORTER_STANDARD_POSTPROCESS_STEPS aiProcess_ImproveCacheLocality | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_Debone 

class ModelImporter
{
	class NamedTransform
	{
	public:
		string name;
		Transform transform;

		NamedTransform() {}

		NamedTransform(aiString ai_string, Transform transform)
		{
			name = string(ai_string.C_Str());
			this->transform = transform;
		}
	};

	Assimp::Importer _importer;
	aiScene *_scene;

	void AddNodeToTreeNew(aiNode *node, TreeNode<NamedTransform>* parent)
	{
		auto child_count = node->mNumChildren;
		for (unsigned int i = 0; i < child_count; ++i)
		{
			auto ai_child = node->mChildren[i];
			auto child = ConvertAiNodeToTreeNodeNew(ai_child);
			parent->AddChildNode(child);
			AddNodeToTreeNew(ai_child, child);
		}
	}

	Transform CastAssimpMatrix4x4ToTransform(aiMatrix4x4 &matrix)
	{
		Transform transform;
		transform.position = vec3(matrix.a4, matrix.b4, matrix.c4);
		transform.rotation = quat_cast(mat3(
			matrix.a1, matrix.b1, matrix.c1,
			matrix.a2, matrix.b2, matrix.c2,
			matrix.a3, matrix.b3, matrix.c3));
		return transform;
	}

	TreeNode<NamedTransform>* ConvertAiNodeToTreeNodeNew(aiNode *node)
	{
		return new TreeNode<NamedTransform>(NamedTransform(node->mName, CastAssimpMatrix4x4ToTransform(node->mTransformation)));
	}

	TreeNode<NamedTransform>* ConvertAiNodesToTreeNew(aiNode *node)
	{
		auto result = ConvertAiNodeToTreeNodeNew(node);
		AddNodeToTreeNew(node, result);
		return result;
	}

	vector<unsigned int> GetMaxBoneIndices(vector<float> &vertex_weights, unsigned int use_skeleton)
	{
		vector<unsigned int> result;
		float current_top = numeric_limits<float>::max();

		for (unsigned int i = 0; i < use_skeleton; ++i)
		{
			float current_max = numeric_limits<float>::min();
			unsigned int same_max = 0;

			for (auto iter = vertex_weights.begin(); iter != vertex_weights.end(); iter++)
			{
				if (*iter >= current_top)
					continue;

				if (current_max < *iter)
				{
					current_max = *iter;
					same_max = 0;
				}
				else if (current_max == *iter)
				{
					++same_max;
				}
			}

			current_top = current_max;
			unsigned int counter = same_max;
			for (unsigned int j = 0; j < vertex_weights.size(); ++j)
			{
				if (current_max == vertex_weights[j])
				{
					result.push_back(j);
					if (result.size() == use_skeleton)
						return result;
					if (--counter < 0)
						break;
				}
			}
			i += same_max;
		}

		return result;
	}

	bool HasBoneInSubTree(TreeNode<NamedTransform>* node, vector<string> bone_names)
	{
		unsigned int counter = 0;
		node->ForEachInSubTree([&](TreeNode<NamedTransform>* node)
		{
			if (IsBoneName(node->value.name, bone_names))
				++counter;
		});
		return counter > 0;
	}

	bool IsBoneName(string name, vector<string> bone_names)
	{
		for (auto iter = bone_names.begin(); iter != bone_names.end(); iter++)
			if (*iter == name)
				return true;
		return false;
	}

	void NormalizeBoneWeights(float* weight_attributes, unsigned int use_skeleton, unsigned int vertex_count)
	{
		for (unsigned int i = 0; i <  vertex_count; i += use_skeleton)
		{
			float sum = 0.0f;
			for (unsigned int j = 0; j < use_skeleton; ++j)
			{
				sum += weight_attributes[i+j];
			}
			for (unsigned int j = 0; j < use_skeleton; ++j)
			{
				weight_attributes[i+j] = weight_attributes[i+j] / sum;
			}
		}
	}
public:
	class MeshManagerLoadException : public exception {};
	class NoSceneException : public exception {};
	class MoreThan4BoneWeightsException : public exception {};

	ModelImporter()
	{
		_scene = nullptr;
	}

	~ModelImporter()
	{
		UnloadScene();
	}

	Mesh GetMesh(unsigned int mesh_index = 0, unsigned int use_skeleton = 4,
		bool use_position = true, bool use_normal = true, bool use_tangent = true, bool use_bitangent = true,
		unsigned int use_uv_up_to_channel = 8, unsigned int use_color_up_to_channel = 8)
	{
		if (!_scene)
			throw NoSceneException();

		auto mesh = _scene->mMeshes[mesh_index];
		Mesh mesh_converted;

		mesh_converted.primitive_type = GL_TRIANGLES;
		mesh_converted.vertex_count = mesh->mNumVertices;

		if (use_position && mesh->HasPositions())
		{
			mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::POSITION);
			VertexAttribute attribute(3, GL_FLOAT, GL_FALSE);
			mesh_converted.vertex_description += attribute;
			mesh_converted.vertex_data.push_back(MemoryBuilder::CopyMemoryToVector<char>((char*)mesh->mVertices, attribute.Size() * mesh_converted.vertex_count));
		}
		if (use_normal && mesh->HasNormals())
		{
			mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::NORMAL);
			VertexAttribute attribute(3, GL_FLOAT, GL_FALSE);
			mesh_converted.vertex_description += attribute;
			mesh_converted.vertex_data.push_back(MemoryBuilder::CopyMemoryToVector<char>((char*)mesh->mNormals, attribute.Size() * mesh_converted.vertex_count));
		}
		if (use_tangent && mesh->HasTangentsAndBitangents())
		{
			mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::TANGENT);
			VertexAttribute attribute(3, GL_FLOAT, GL_FALSE);
			mesh_converted.vertex_description += attribute;
			mesh_converted.vertex_data.push_back(MemoryBuilder::CopyMemoryToVector<char>((char*)mesh->mTangents, attribute.Size() * mesh_converted.vertex_count));
		}
		if (use_bitangent && mesh->HasTangentsAndBitangents())
		{
			mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BITANGENT);
			VertexAttribute attribute(3, GL_FLOAT, GL_FALSE);
			mesh_converted.vertex_description += attribute;
			mesh_converted.vertex_data.push_back(MemoryBuilder::CopyMemoryToVector<char>((char*)mesh->mBitangents, attribute.Size() * mesh_converted.vertex_count));
		}
		if (use_uv_up_to_channel)
		{
			auto uv_channels_to_load = (use_uv_up_to_channel < mesh->GetNumUVChannels()) ?  use_uv_up_to_channel : mesh->GetNumUVChannels();
			for (unsigned int i = 0; i < uv_channels_to_load; ++i)
			{
				if (!mesh->HasTextureCoords(i))
					continue;
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::UV);
				VertexAttribute attribute(2, GL_FLOAT, GL_FALSE);
				mesh_converted.vertex_description += attribute;
				mesh_converted.vertex_data.push_back(MemoryBuilder::CopyMemoryToVector<char>((char*)mesh->mTextureCoords[i], attribute.Size() * mesh_converted.vertex_count));
			}
		}
		if (use_color_up_to_channel)
		{
			auto color_channels_to_load = (use_color_up_to_channel < mesh->GetNumColorChannels()) ?  use_color_up_to_channel : mesh->GetNumColorChannels();
			for (unsigned int i = 0; i < color_channels_to_load; ++i)
			{
				if (!mesh->HasVertexColors(i))
					continue;
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::COLOR);
				VertexAttribute attribute(4, GL_FLOAT, GL_FALSE);
				mesh_converted.vertex_description += attribute;
				mesh_converted.vertex_data.push_back(MemoryBuilder::CopyMemoryToVector<char>((char*)mesh->mColors[i], attribute.Size() * mesh_converted.vertex_count));
			}
		}

		if (use_skeleton > 0 && mesh->HasBones())
		{
			auto bone_indices = new vector<GLuint>[mesh->mNumVertices];
			auto bone_weights = new vector<GLfloat>[mesh->mNumVertices];

			for (unsigned int i = 0; i < mesh->mNumBones; ++i)
			{
				auto bone = mesh->mBones[i];
				for (unsigned int j = 0; j < bone->mNumWeights; ++j)
				{
					auto weight = bone->mWeights[j];
					bone_indices[weight.mVertexId].push_back(i);
					bone_weights[weight.mVertexId].push_back(weight.mWeight);
				}
			}

			VertexAttribute attribute_id(use_skeleton, GL_UNSIGNED_INT, GL_FALSE);
			mesh_converted.vertex_description += attribute_id;
			VertexAttribute attribute_weight(use_skeleton, GL_FLOAT, GL_FALSE);			
			mesh_converted.vertex_description += attribute_weight;

			switch (use_skeleton)
			{
			case 1:
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BONEID_1);
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BONEWEIGHT_1);
				break;
			case 2:
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BONEID_2);
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BONEWEIGHT_2);
				break;
			case 3:
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BONEID_3);
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BONEWEIGHT_3);
				break;
			case 4:
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BONEID_4);
				mesh_converted.mesh_attributes.push_back(Mesh::MeshAttributeType::BONEWEIGHT_4);
				break;
			default:
				throw MoreThan4BoneWeightsException();
				break;
			}
			
			auto index_attributes = new GLuint[mesh->mNumVertices * use_skeleton];
			auto weight_attributes = new GLfloat[mesh->mNumVertices * use_skeleton];
			memset(index_attributes, 0, mesh->mNumVertices * attribute_id.Size());
			memset(weight_attributes, 0, mesh->mNumVertices * attribute_weight.Size());
			for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			{
				auto max_indices = GetMaxBoneIndices(bone_weights[i], use_skeleton);
				unsigned j = 0;
				for (auto iter = max_indices.begin(); iter != max_indices.end(); iter++)
				{
					index_attributes[i * use_skeleton + j] = bone_indices[i][*iter];
					weight_attributes[i * use_skeleton + j] = bone_weights[i][*iter];
					++j;
				}
			}
			NormalizeBoneWeights(weight_attributes, use_skeleton, mesh->mNumVertices);

			for (unsigned int i = 0; i < mesh->mNumVertices; i+=4)
			{
				cout << index_attributes[i+0] << " " << index_attributes[i+1] << " " << index_attributes[i+2] << " " << index_attributes[i+3] << " "<< endl;
			}

			mesh_converted.vertex_data.push_back(MemoryBuilder::CopyMemoryToVector<char>((char*)index_attributes, attribute_id.Size() * mesh_converted.vertex_count));
			delete[] index_attributes;
			mesh_converted.vertex_data.push_back(MemoryBuilder::CopyMemoryToVector<char>((char*)weight_attributes, attribute_weight.Size() * mesh_converted.vertex_count));
			delete[] weight_attributes;

			delete[] bone_indices;
			delete[] bone_weights;
		}

		unsigned int max_index = 0;
		for (unsigned int i = 0 ; i < mesh->mNumFaces ; ++i)
		{
			const aiFace &face = mesh->mFaces[i];
			mesh_converted.indices.push_back(face.mIndices[0]);
			mesh_converted.indices.push_back(face.mIndices[1]);
			mesh_converted.indices.push_back(face.mIndices[2]);
			max_index = std::max(std::max(max_index, face.mIndices[0]), std::max(face.mIndices[1], face.mIndices[2]));
		}
		mesh_converted.max_index = max_index;
		return mesh_converted;
	}

	vector<Mesh> GetMeshes(unsigned int use_skeleton = 4, bool use_position = true, bool use_normal = true, bool use_tangent = true, bool use_bitangent = true,
		unsigned int use_uv_up_to_channel = 8, unsigned int use_color_up_to_channel = 8)
	{
		vector<Mesh> result;
		for (unsigned int i = 0; i < _scene->mNumMeshes; ++i)
			result.push_back(GetMesh(i, use_skeleton, use_position, use_normal, use_tangent, use_bitangent, use_uv_up_to_channel, use_color_up_to_channel));
		return result;
	}

	Skeleton* GetMeshSkeletonNew(unsigned int mesh_index)
	{
		if (!_scene)
			throw NoSceneException();

		auto mesh = _scene->mMeshes[mesh_index];
		if (!mesh->HasBones())
			return nullptr;

		vector<Transform> bone_offsets;
		vector<string> bone_names;
		for (unsigned int i = 0; i < mesh->mNumBones; ++i)
		{
			bone_names.push_back(string(mesh->mBones[i]->mName.C_Str()));
			bone_offsets.push_back(CastAssimpMatrix4x4ToTransform(mesh->mBones[i]->mOffsetMatrix));
		}

		auto tree = ConvertAiNodesToTreeNew(_scene->mRootNode);
		vector<TreeNode<NamedTransform>*> release_tree_nodes;
		tree->ForEach([&](TreeNode<NamedTransform>* node)
		{
			if (!IsBoneName(node->value.name, bone_names))
			{
				if (!HasBoneInSubTree(node, bone_names))
				{
					release_tree_nodes.push_back(node);
				}
				else if (node->value.transform.IsIdentity(.0001f, .0001f))
				{
					release_tree_nodes.push_back(node);
				}
			}
		});

		TreeNode<NamedTransform>* root = nullptr;
		for (auto iter = release_tree_nodes.begin(); iter != release_tree_nodes.end(); iter++)
		{
			auto node = *iter;
			if (node->IsRoot())
				root = node;
			else
			{
				node->RemoveFromTree();
				delete node;
			}
		}
		if (root)
		{
			if (root->children.size() == 1)
			{
				tree = root->children[0];
				root->RemoveFromTree();
				delete root;
			}
		}

		auto casted_tree = tree->CloneTreeNew<Transform>([](NamedTransform value){return value.transform;});
		auto flat_tree_values = tree->GetBreadthFirstSearchValues();
		auto flat_casted_tree = casted_tree->GetBreadthFirstSearchFringe();
		vector<TreeNode<Transform>*> bones;
		for (unsigned int i = 0; i < flat_tree_values.size(); ++i)
			if (IsBoneName(flat_tree_values[i].name, bone_names))
				bones.push_back(flat_casted_tree[i]);
		tree->ReleaseTree();

		return new Skeleton(bones, bone_offsets);
	}

	void LoadSceneFromFile(string file_path,
		unsigned int post_process_steps = MODEL_IMPORTER_STANDARD_POSTPROCESS_STEPS)
	{
		UnloadScene();
		_scene = (aiScene *)_importer.ReadFile(file_path, aiProcess_Triangulate | post_process_steps);
		if (!_scene)
		{
			throw MeshManagerLoadException();
			cout << _importer.GetErrorString() << endl;
		}
	}

	void LoadSceneFromMemory(void *data, size_t data_size,
		unsigned int post_process_steps = MODEL_IMPORTER_STANDARD_POSTPROCESS_STEPS)
	{
		UnloadScene();
		_scene = (aiScene *)_importer.ReadFileFromMemory(data, data_size, aiProcess_Triangulate | post_process_steps);
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
};

#endif
