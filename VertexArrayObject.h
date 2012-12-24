#ifndef vertex_array_object_h__
#define vertex_array_object_h__

#include "vertex_description.h"

// GL_OES_vertex_array_object must be supported
class VertexArrayObject
{
    GLuint _vao, _vbo, _ibo;
    GLenum _primitiveMode, _indexType;
    GLsizei _vertexCount, _vertexSize, _indexCount;
    
public:
    VertexArrayObject()
    {
        _vbo = 0;
        _ibo = 0;
        
        glGenVertexArraysOES(1, &_vao);
    }
    
    ~VertexArrayObject() 
    {
        if (_ibo) glDeleteBuffers(1, &_ibo);
        if (_vbo) glDeleteBuffers(1, &_vbo);
        if (_vao) glDeleteVertexArraysOES(1, &_vao);
    }
    
    GLuint VAO()
    {
        return _vao;
    }
    GLuint VBO()
    {
        return _vbo;
    }
    GLuint IBO()
    {
        return _ibo;
    }

    void AddPackedVertices(GLenum vertexUsage, GLenum primitiveMode, VertexDescription vertexDescription, GLvoid *data, GLsizei vertexCount)
    {
        if (_vbo) glDeleteBuffers(1, &_vbo);

        _primitiveMode = primitiveMode;
        _vertexCount = vertexCount;
        _vertexSize = vertexDescription.VertexSize();
        glBindVertexArrayOES(_vao);

        glGenBuffers(1, &_vbo);	
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexDescription.VertexSize() * vertexCount, data, vertexUsage);

        for (unsigned int i = 0; i < vertexDescription.AttributeCount(); ++i)
        {
            VertexAttribute attribute = vertexDescription[i];

            glVertexAttribPointer(i, attribute.ComponentCount, attribute.ComponentType, attribute.Normalized, _vertexSize, vertexDescription.AttributeOffset(i));
            glEnableVertexAttribArray(i);
        }
    }
    GLvoid* BeginMapVertices(GLenum access = GL_WRITE_ONLY_OES)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        if (access == GL_WRITE_ONLY_OES) glBufferData(GL_ARRAY_BUFFER, _vertexCount * _vertexSize, NULL, GL_DYNAMIC_DRAW);
        return glMapBufferOES(GL_ARRAY_BUFFER, access);
    }
    void EndMapVertices()
    {
        glUnmapBufferOES(GL_ARRAY_BUFFER);
    }
    
    void AddIndices(GLenum indexUsage, GLenum indexType, GLvoid *data, GLsizei indexCount)
    {
        if (_ibo) glDeleteBuffers(1, &_ibo);

        _indexType = indexType;
        _indexCount = indexCount;

        glBindVertexArrayOES(_vao);
        
        glGenBuffers(1, &_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, OpenGLUtility::SizeofTypeByEnum(indexType) * indexCount, data, indexUsage);
    }
    GLvoid* BeginMapIndices(GLenum access = GL_WRITE_ONLY_OES)
    {   
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        if (access == GL_WRITE_ONLY_OES) glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * OpenGLUtility::SizeofTypeByEnum(_indexType), NULL, GL_DYNAMIC_DRAW);
        return glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, access);
    }
    void EndMapIndices()
    {
        glUnmapBufferOES(GL_ELEMENT_ARRAY_BUFFER);
    }
    
    void Draw()
    {
        glBindVertexArrayOES(_vao);
        
        if (_ibo)
        {
            glDrawElements(_primitiveMode, _indexCount, _indexType, 0);
        }
        else
        {
            glDrawArrays(_primitiveMode, 0, _vertexCount);
        }
    }
    void DrawIndexed()
    {
        glBindVertexArrayOES(_vao);
        glDrawElements(_primitiveMode, _indexCount, _indexType, 0);
    }
    void DrawRaw()
    {
        glBindVertexArrayOES(_vao);
        glDrawArrays(_primitiveMode, 0, _vertexCount);
    }
};

#endif
