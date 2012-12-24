#ifndef vertex_attribute_h__
#define vertex_attribute_h__

#include <OpenGLES/ES2/gl.h>

#include "opengl.h"

class VertexAttribute
{
    GLsizei component_count;
    GLenum component_type; 
    GLboolean normalized;

    VertexAttribute(GLsizei component_count, GLenum component_type, GLboolean normalized)
    {
        this.component_count = component_count;
        this.component_type = component_type;
        this.normalized = normalized;
    }
    
    GLsizei AttributeSize()
    {
        return component_count * OpenGL::SizeofTypeByEnum(component_type);
    }
};

#endif