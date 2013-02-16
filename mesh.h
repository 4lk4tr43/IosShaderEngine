#ifndef mesh_h__
#define mesh_h__

#include <vector>
using namespace std;

#include "opengl.h"
#include "transform.h"
#include "vertex_array_object.h"

class Mesh
{
	class NoBonesException : public exception {};

	void AddCastedIndicesToVertexArrayObject(VertexArrayObject *vao, GLenum index_usage, GLenum index_type = GL_NONE)
	{
		if (index_type == GL_NONE)
		{
			if (max_index < 256)
				index_type = GL_UNSIGNED_BYTE;
			else if (max_index < 65536)
				index_type = GL_UNSIGNED_SHORT;
			else
				index_type = GL_UNSIGNED_INT;
		}

		if (index_type == GL_UNSIGNED_BYTE)
		{
			vector<GLubyte> indices_casted;
			for (size_t i = 0; i < indices.size(); ++i)
				indices_casted.push_back((GLubyte)indices[i]);
			vao->AddIndices(index_usage, GL_UNSIGNED_BYTE, (GLvoid*)&indices_casted[0], indices.size());
		}
		else if (index_type == GL_UNSIGNED_SHORT)
		{
			vector<GLushort> indices_casted;
			for (size_t i = 0; i < indices.size(); ++i)
				indices_casted.push_back((GLushort)indices[i]);
			vao->AddIndices(index_usage, GL_UNSIGNED_SHORT, (GLvoid*)&indices_casted[0], indices.size());
		}
		else if (index_type == GL_UNSIGNED_INT)
		{
			vector<GLuint> indices_casted;
			for (size_t i = 0; i < indices.size(); ++i)
				indices_casted.push_back((GLuint)indices[i]);
			vao->AddIndices(index_usage, GL_UNSIGNED_INT, (GLvoid*)&indices_casted[0], indices.size());
		}
	}

	vector<char> CastBoneVertexIDs(GLenum bone_index_type = GL_UNSIGNED_INT)
	{
		vector<char> bone_ids;
		for (unsigned int i = 0; i < mesh_attributes.size(); ++i)
		{
			if (mesh_attributes[i] == MeshAttributeType::BONEID_1 || mesh_attributes[i] == MeshAttributeType::BONEID_2 ||
				mesh_attributes[i] == MeshAttributeType::BONEID_3 || mesh_attributes[i] == MeshAttributeType::BONEID_4)
			{
				if (bone_index_type == GL_UNSIGNED_INT)
					return vertex_data[i];

				bone_ids = vertex_data[i];
				unsigned int *bone_ids_uint_array = (unsigned int *)&bone_ids[0];
				unsigned int bone_id_count = sizeof(char) * bone_ids.size() / sizeof(unsigned int);

				if (bone_index_type == GL_UNSIGNED_BYTE)
				{
					auto memory = new GLubyte[bone_id_count];
					for (unsigned int j = 0; j < bone_id_count; ++j)
						memory[j] = (GLubyte)bone_ids_uint_array[j];
					vertex_data[i] = MemoryBuilder::CopyMemoryToVector<char>((char*)memory, bone_id_count);
					delete[] memory;
				}
				else if(bone_index_type == GL_UNSIGNED_SHORT)
				{
					auto memory = new GLushort[bone_id_count];
					for (unsigned int j = 0; j < bone_id_count; ++j)				{
						memory[j] = (GLushort)bone_ids_uint_array[j];				}
					vertex_data[i] = MemoryBuilder::CopyMemoryToVector<char>((char*)memory, bone_id_count);
					delete[] memory;
				}
				return bone_ids;
			}
		}
		throw NoBonesException();
	}

	void RestoreUncastedBoneIndexType(vector<char> &uncasted_bone_indices)
	{
		for (unsigned int i = 0; i < mesh_attributes.size(); ++i)
		{
			if (mesh_attributes[i] == MeshAttributeType::BONEID_1 || mesh_attributes[i] == MeshAttributeType::BONEID_2 ||
				mesh_attributes[i] == MeshAttributeType::BONEID_3 || mesh_attributes[i] == MeshAttributeType::BONEID_4)
			{
				vertex_data[i] = uncasted_bone_indices;
				return;
			}
		}
	}

public:
	enum MeshAttributeType
	{
		POSITION,
		NORMAL,
		TANGENT,
		BITANGENT,
		UV,
		COLOR,
		BONEID_1,
		BONEWEIGHT_1,
		BONEID_2,
		BONEWEIGHT_2,
		BONEID_3,
		BONEWEIGHT_3,
		BONEID_4,
		BONEWEIGHT_4
	};

	GLenum primitive_type;
	unsigned int vertex_count;
	unsigned int max_index;
	vector<MeshAttributeType> mesh_attributes;
	vector<unsigned int> indices;
	vector<vector<char>> vertex_data;
	VertexDescription vertex_description;

	VertexArrayObject* ConvertToVertexArrayObjectNew(GLenum vertex_usage = GL_STATIC_DRAW, GLenum index_usage = GL_STATIC_DRAW,
		bool pack_vertex_data = false, GLenum bone_index_type = GL_UNSIGNED_BYTE, GLenum index_type = GL_NONE)
	{
		vector<char> uncasted_bone_indices;
		bool no_bones = false;
		try
		{
			uncasted_bone_indices = CastBoneVertexIDs(bone_index_type);
		}
		catch (NoBonesException &)
		{
			no_bones = true;
		}
		auto vao = new VertexArrayObject();
		if (pack_vertex_data)
		{
			vector<char> packed_data;
			for (size_t i = 0; i < vertex_count; ++i)
				for (size_t j = 0; j < vertex_data.size(); ++j)
					packed_data.push_back(vertex_data[j][i]);
			vao->AddPackedVertices(vertex_usage, primitive_type, vertex_description, (GLvoid*)&packed_data[0], vertex_count);
		}
		else
		{
			vector<void*> casted_vertex_data;
			for (size_t i = 0; i < vertex_data.size(); ++i)
				casted_vertex_data.push_back((GLvoid*)&vertex_data[i][0]);
			vao->AddVertices(vertex_usage, primitive_type, vertex_description, casted_vertex_data, vertex_count);
		}
		if (!no_bones)
			RestoreUncastedBoneIndexType(uncasted_bone_indices);
		AddCastedIndicesToVertexArrayObject(vao, index_usage, index_type);
		return vao;
	}

	VertexArrayObject* ConvertToVertexArrayObjectNew(vector<GLenum> vertex_usages, GLenum index_usage = GL_STATIC_DRAW,
		GLenum bone_index_type = GL_UNSIGNED_INT, GLenum index_type = GL_NONE)
	{
		vector<char> uncasted_bone_indices;
		bool no_bones = false;
		try
		{
			uncasted_bone_indices = CastBoneVertexIDs(bone_index_type);
		}
		catch (NoBonesException &)
		{
			no_bones = true;
		}
		auto vao = new VertexArrayObject();
		vector<void*> casted_vertex_data;
		for (size_t i = 0; i < vertex_data.size(); ++i)
			casted_vertex_data.push_back((GLvoid*)&vertex_data[i][0]);
		vao->AddVertices(vertex_usages, primitive_type, vertex_description, casted_vertex_data, vertex_count);
		if (!no_bones)
			RestoreUncastedBoneIndexType(uncasted_bone_indices);
		AddCastedIndicesToVertexArrayObject(vao, index_usage, index_type);
		return vao;
	}

	static Mesh Deserialize(char *serialized_data)
	{
		Mesh mesh;
		MemoryBuilder memory_builder;
		memory_builder.PointToSource(serialized_data);
		mesh.primitive_type = *(GLenum*)memory_builder.Pop();
		mesh.vertex_count = *(unsigned int*)memory_builder.Pop();
		mesh.max_index = *(unsigned int*)memory_builder.Pop();

		size_t size;
		auto mesh_attributes = (MeshAttributeType*)memory_builder.Pop(&size);
		for (unsigned int i = 0; i < size / sizeof(MeshAttributeType); ++i)
			mesh.mesh_attributes.push_back(mesh_attributes[i]);
		auto indices = (unsigned int*)memory_builder.Pop(&size);
		for (unsigned int i = 0; i < size / sizeof(unsigned int); ++i)
			mesh.indices.push_back(indices[i]);
		mesh.vertex_description = VertexDescription::Deserialize(memory_builder.Pop());
		while (true)
		{
			auto attribute_data = memory_builder.Pop(&size);
			if (!attribute_data)
				break;
			vector<char> attribute_data_to_add;
			for (size_t i = 0; i < size; ++i)
				attribute_data_to_add.push_back(attribute_data[i]);
			mesh.vertex_data.push_back(attribute_data_to_add);
		}
		return mesh;
	}

	static Mesh* DeserializeNew(char *serialized_data)
	{
		return new Mesh(Deserialize(serialized_data));
	}

	char* SerializeNew(size_t* size = nullptr)
	{
		MemoryBuilder memory_builder;
		memory_builder.Push(&primitive_type, sizeof(GLenum));
		memory_builder.Push(&vertex_count, sizeof(unsigned int));
		memory_builder.Push(&max_index, sizeof(unsigned int));
		memory_builder.Push(&mesh_attributes[0], sizeof(MeshAttributeType) * mesh_attributes.size());
		memory_builder.Push(&indices[0], sizeof(unsigned int) * indices.size());
		size_t serialized_vertex_description_size;
		auto serialized_vertex_description = vertex_description.SerializeNew(&serialized_vertex_description_size);
		memory_builder.Push(serialized_vertex_description, serialized_vertex_description_size);
		delete[] serialized_vertex_description;
		for (size_t i = 0; i < vertex_data.size(); ++i)
			memory_builder.Push(&vertex_data[i][0], vertex_data[i].size());
		return memory_builder.GetMemoryNew(size);
	}

	void Skin(vector<Transform> bone_joints, unsigned short influence_count)
	{
		// TODO
		throw std::exception();
	}

	void Strip()
	{
		if (primitive_type == GL_TRIANGLE_STRIP)
			return;

		primitive_type = GL_TRIANGLE_STRIP;

		// TODO
		throw std::exception();
	}
};

#endif
