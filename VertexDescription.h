#ifndef VertexDescription_h__
#define VertexDescription_h__ VertexDescription_h__

#include <OpenGLES/ES2/gl.h>
#include <vector>
using namespace std;

#include "../Types/Vector3.h"
#include "VertexAttribute.h"

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) (GLvoid *)((char *)NULL + (i))
#endif

namespace Eos
{
    namespace OpenGL
    {
        class VertexDescription
        {
            GLsizei _vertexSize;
            vector<VertexAttribute> _attributeDescriptions;

        public:
            VertexDescription()
            {
                _vertexSize = 0;
            }
            static VertexDescription Position()
            {
                VertexDescription vertexDescription;

                vertexDescription.AddAttribute(3, GL_FLOAT, GL_FALSE);

                return vertexDescription;
            }
            static VertexDescription PositionUV()
            {
                VertexDescription vertexDescription = VertexDescription::Position();

                vertexDescription.AddAttribute(2, GL_FLOAT, GL_FALSE);

                return vertexDescription;
            }
            static VertexDescription PositionNormal()
            {
                VertexDescription vertexDescription = VertexDescription::Position();

                vertexDescription.AddAttribute(3, GL_FLOAT, GL_FALSE);

                return vertexDescription;
            }
            static VertexDescription PositionUVNormal()
            {
                VertexDescription vertexDescription = VertexDescription::PositionUV();

                vertexDescription.AddAttribute(3, GL_FLOAT, GL_FALSE);

                return vertexDescription;
            }

            void operator+=(VertexAttribute vertexAttribute)
            {
                _attributeDescriptions.push_back(vertexAttribute);
                _vertexSize += vertexAttribute.AttributeSize();
            }
            void AddAttribute(GLint componentCount, GLenum componentType, GLboolean normalized)
            {
                VertexAttribute vertexAttribute = VertexAttribute(componentCount, componentType, normalized);
                operator+=(vertexAttribute);		
            }
            VertexAttribute operator[](int index)
            {
                return _attributeDescriptions[index];
            }
            
            GLsizei AttributeCount()
            {
                return _attributeDescriptions.size();
            }
            GLsizei VertexSize()
            {
                return _vertexSize;
            }
            GLvoid * AttributeOffset(GLuint attributeIndex)
            {
                GLsizei offset = 0;
                
                for (unsigned int i = 0; i < attributeIndex; ++i) offset += _attributeDescriptions[i].AttributeSize();
                
                return BUFFER_OFFSET(offset);
            }
            
            char * Serialize(unsigned long *memorySizeInBytes) // memory must delete[] manually
            {
                GLsizei attributeCount = _attributeDescriptions.size();
                *memorySizeInBytes = sizeof(unsigned long) + sizeof(GLsizei) +  attributeCount * sizeof(VertexAttribute);
                char *memory = new char[*memorySizeInBytes];
                
                memory[0] = *memorySizeInBytes;                
                memory[sizeof(unsigned long)] = attributeCount;
                
                for (GLsizei i = 0; i < attributeCount; ++i) 
                {
                    memcpy(&memory[sizeof(unsigned long) + sizeof(GLsizei) + i * sizeof(VertexAttribute)], &_attributeDescriptions[i], sizeof(VertexAttribute));
                }
                
                return memory;
            }        
            static VertexDescription * Deserialize(char *memory) // must delete manually
            {
                VertexDescription *vertexDescription = new VertexDescription();
                
                GLsizei attributeCount = *(GLsizei *)&memory[sizeof(unsigned long)];
                VertexAttribute *attributes = (VertexAttribute *)&memory[sizeof(unsigned long) + sizeof(GLsizei)];
                for (GLsizei i = 0; i < attributeCount; ++i) vertexDescription->operator+=(attributes[i]);
                
                return vertexDescription;
            }
        };
    }
}

#endif // VertexDescription_h__
