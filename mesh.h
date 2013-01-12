#ifndef mesh_h__
#define mesh_h__

#include <vector>
using namespace std;

#include "opengl.h"
#include "transform.h"
#include "vertex_array_object.h"

class Mesh
{
	void AddCastedIndicesToVertexArrayObject(VertexArrayObject *vao, GLenum index_usage) 
	{
		if (max_index < 256)
		{
			vector<GLubyte> indices_casted;
			for (size_t i = 0; i < indices.size(); ++i)
				indices_casted.push_back((GLubyte)indices[i]);
			vao->AddIndices(index_usage, GL_UNSIGNED_BYTE, (GLvoid*)&indices_casted[0], indices.size());
		}
		else if (max_index < 65536)
		{
			vector<GLushort> indices_casted;
			for (size_t i = 0; i < indices.size(); ++i)
				indices_casted.push_back((GLushort)indices[i]);
			vao->AddIndices(index_usage, GL_UNSIGNED_SHORT, (GLvoid*)&indices_casted[0], indices.size());
		}
		else
			vao->AddIndices(index_usage, GL_UNSIGNED_INT, (GLvoid*)&indices[0], indices.size());
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
		BONEWEIGHT_4,
	};

	GLenum primitive_type;
	unsigned int vertex_count;
	unsigned int max_index;
	vector<MeshAttributeType> mesh_attributes;
	vector<unsigned int> indices;
	vector<vector<char>> vertex_data;
	VertexDescription vertex_description;	

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

	VertexArrayObject* ConvertToVertexArrayObjectNew(GLenum vertex_usage = GL_STATIC_DRAW, GLenum index_usage = GL_STATIC_DRAW,	bool pack_vertex_data = false)
	{
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
		AddCastedIndicesToVertexArrayObject(vao, index_usage);
		return vao;
	}

	VertexArrayObject* ConvertToVertexArrayObjectNew(vector<GLenum> vertex_usages, GLenum index_usage = GL_STATIC_DRAW)
	{
		auto vao = new VertexArrayObject();
		vector<void*> casted_vertex_data;
		for (size_t i = 0; i < vertex_data.size(); ++i)
			casted_vertex_data.push_back((GLvoid*)&vertex_data[i][0]);
		vao->AddVertices(vertex_usages, primitive_type, vertex_description, casted_vertex_data, vertex_count);
		AddCastedIndicesToVertexArrayObject(vao, index_usage);
		return vao;
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
