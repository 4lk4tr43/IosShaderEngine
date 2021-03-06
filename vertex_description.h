#ifndef vertex_description_h__
#define vertex_description_h__

#include <vector>
using namespace std;

#include "memory_builder.h"
#include "vertex_attribute.h"

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) (GLvoid*)((char*)nullptr + (i))
#endif

class VertexDescription
{    
    GLsizei _vertex_size;
	vector<VertexAttribute> _attribute_descriptions;

public:
	char* SerializeNew(size_t *size = nullptr)
	{
		MemoryBuilder memory_builder;
		for (auto iter = _attribute_descriptions.begin(); iter != _attribute_descriptions.end(); iter++)
			memory_builder.Push(&*iter, sizeof(VertexAttribute));
		return memory_builder.GetMemoryNew(size);
	}

	static VertexDescription Deserialize(char *serialized_data)
	{
		VertexDescription vertex_description;
		MemoryBuilder memory_builder;
		memory_builder.PointToSource(serialized_data);		
		while(true) 
		{
			auto attribute = (VertexAttribute*)memory_builder.Pop();
			if (!attribute)
				break;
			vertex_description += *attribute;
		}
		return vertex_description;
	}

    VertexDescription()
    {
        _vertex_size = 0;
    }
    
    static VertexDescription Position()
    {
        VertexDescription vertex_description;
        vertex_description.AddAttribute(3, GL_FLOAT, GL_FALSE);
        return vertex_description;
    }
    
	static VertexDescription PositionColor()
	{
		VertexDescription vertex_description = VertexDescription::Position();
		vertex_description.AddAttribute(4, GL_FLOAT, GL_FALSE);
		return vertex_description;
	}

    static VertexDescription PositionNormal()
    {
        VertexDescription vertex_description = VertexDescription::Position();
        vertex_description.AddAttribute(3, GL_FLOAT, GL_FALSE);
        return vertex_description;
    }
    
    static VertexDescription PositionUV()
    {
        VertexDescription vertex_description = VertexDescription::Position();
        vertex_description.AddAttribute(2, GL_FLOAT, GL_FALSE);
        return vertex_description;
    }
    
    static VertexDescription PositionUVNormal()
    {
        VertexDescription vertex_description = VertexDescription::PositionUV();
        vertex_description.AddAttribute(3, GL_FLOAT, GL_FALSE);
        return vertex_description;
    }
    
    void AddAttribute(GLint component_count, GLenum component_type, GLboolean normalized)
    {
        VertexAttribute vertex_attribute(component_count, component_type, normalized);
        operator+=(vertex_attribute);		
    }
    
    GLsizei AttributeCount()
    {
        return _attribute_descriptions.size();
    }
    
    GLvoid* AttributeOffset(GLuint attributeIndex)
    {
        GLsizei offset = 0;
        for (unsigned int i = 0; i < attributeIndex; ++i)
            offset += _attribute_descriptions[i].Size();
        return BUFFER_OFFSET(offset);
    }
    
    VertexAttribute operator[](int index)
    {
        return _attribute_descriptions[index];
    }
    
    void operator+=(VertexAttribute vertexAttribute)
    {
        _attribute_descriptions.push_back(vertexAttribute);
        _vertex_size += vertexAttribute.Size();
    }
    
    GLsizei Size()
    {
        return _vertex_size;
    }
};

#endif
