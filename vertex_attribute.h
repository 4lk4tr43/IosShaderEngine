#ifndef vertex_attribute_h__
#define vertex_attribute_h__

#include "opengl.h"

class VertexAttribute
{
public:
	GLboolean normalized;
    GLenum component_type; 
    GLsizei component_count;

    VertexAttribute(GLsizei component_count, GLenum component_type, GLboolean normalized)
    {
        this->component_count = component_count;
        this->component_type = component_type;
        this->normalized = normalized;
    }
    
    GLsizei Size()
    {
        return component_count * OpenGL::SizeofTypeByEnum(component_type);
    }
};

#endif