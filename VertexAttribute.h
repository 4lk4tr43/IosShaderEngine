#ifndef VertexAttribute_h__
#define VertexAttribute_h__ VertexAttribute_h__

#include <OpenGLES/ES2/gl.h>

#include "OpenGLUtility.h"

namespace Eos
{
    namespace OpenGL
    {
        struct VertexAttribute
        {
            GLsizei ComponentCount;
            GLenum ComponentType; 
            GLboolean Normalized;

            VertexAttribute(GLsizei componentCount, GLenum componentType, GLboolean normalized)
            {
                ComponentCount = componentCount;
                ComponentType = componentType;
                Normalized = normalized;
            }
            
            GLsizei AttributeSize()
            {
                return ComponentCount * OpenGLUtility::SizeofTypeByEnum(ComponentType);
            }
        };
    }
}

#endif // VertexAttribute_h__