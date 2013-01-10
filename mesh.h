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
			for (int i = 0; i < indices.size(); ++i)
				indices_casted.push_back((GLubyte)indices[i]);
			vao->AddIndices(index_usage, GL_UNSIGNED_BYTE, (GLvoid*)&indices_casted[0], indices_casted.size());
		}
		else if (max_index < 65536)
		{
			vector<GLushort> indices_casted;
			for (int i = 0; i < indices.size(); ++i)
				indices_casted.push_back((GLushort)indices[i]);
			vao->AddIndices(index_usage, GL_UNSIGNED_SHORT, (GLvoid*)&indices_casted[0], indices_casted.size());
		}
		else
			vao->AddIndices(index_usage, GL_UNSIGNED_INT, (GLvoid*)&indices[0], indices.size());
	}

	template <class T>
	vector<T> ConvertMemoryToVector(T* memory, size_t element_count)
	{
		return vector<T>(memory, memory + element_count);
	}

	size_t SizeOfAttributeBuffer(unsigned int index)
	{
		return vertex_count * vertex_description[index].Size();
	}

	void FixDataStructure()
	{
		if (decoupled_vertex_data.size())
		{
			CorrelateVertexDataToDecoupledVertexData();
		}
		else if (vertex_data.size())
		{
			DecoupleVertexData();
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
		BONEWEIGHT_4,
	};

	GLenum primitive_type;
	unsigned int vertex_count;
	unsigned int max_index;
	vector<MeshAttributeType> mesh_attributes;
	vector<unsigned int> indices;
	vector<void*> vertex_data;
	vector<vector<char>> decoupled_vertex_data;
	VertexDescription vertex_description;	

	VertexArrayObject* ConvertToVertexArrayObjectNew(GLenum vertex_usage = GL_STATIC_DRAW, GLenum index_usage = GL_STATIC_DRAW,	bool pack_vertex_data = false)
	{
		CorrelateVertexDataToDecoupledVertexData();
		auto vao = new VertexArrayObject();
		if (pack_vertex_data)
		{
			vector<char> packed_data;
			for (int i = 0; i < vertex_count; ++i)
				for (int j = 0; j < decoupled_vertex_data.size(); ++j)
					packed_data.push_back(decoupled_vertex_data[j][i]);
			vao->AddPackedVertices(vertex_usage, primitive_type, vertex_description, (GLvoid*)&packed_data[0], vertex_count);
		}
		else
			vao->AddVertices(vertex_usage, primitive_type, vertex_description, vertex_data, vertex_count);
			
		AddCastedIndicesToVertexArrayObject(vao, index_usage);
		return vao;
	}

	VertexArrayObject* ConvertToVertexArrayObjectNew(vector<GLenum> vertex_usages, GLenum index_usage = GL_STATIC_DRAW)
	{
		auto vao = new VertexArrayObject();
		vao->AddVertices(vertex_usages, primitive_type, vertex_description, vertex_data, vertex_count);
		AddCastedIndicesToVertexArrayObject(vao, index_usage);
		return vao;
	}	

	void BuildDecoupledVertexData()
	{		
		for (int i = 0; i < vertex_data.size(); ++i)
		{			
			auto data_to_push = (char*)vertex_data[i];
			vector<char> data = ConvertMemoryToVector<char>(data_to_push, SizeOfAttributeBuffer(i));
			decoupled_vertex_data.push_back(data);
		}		
	}

	void CorrelateVertexDataToDecoupledVertexData()
	{
		if (!decoupled_vertex_data.size())
			return;
		vertex_data.clear();
		for (int i = 0; i < decoupled_vertex_data.size(); ++i)
			vertex_data.push_back((void*)&decoupled_vertex_data[i][0]);
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
