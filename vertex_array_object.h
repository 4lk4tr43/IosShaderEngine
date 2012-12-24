#ifndef vertex_array_object_h__
#define vertex_array_object_h__

#include "vertex_description.h"

#ifdef WIN32
	#define glGenVertexArraysOES glGenVertexArrays
	#define glDeleteVertexArraysOES glDeleteVertexArrays
	#define glBindVertexArrayOES glBindVertexArray
	#define glMapBufferOES glMapBuffer
	#define glUnmapBufferOES glUnmapBuffer
	#define GL_WRITE_ONLY_OES GL_WRITE_ONLY
#endif

// GL_OES_vertex_array_object must be supported
class VertexArrayObject
{
    GLenum _index_type, _primitive_mode;
    GLsizei _index_count, _vertex_count, _vertex_size;
    GLuint _ibo, _vao, _vbo;

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
    
    void AddIndices(GLenum indexUsage, GLenum indexType, GLvoid *data, GLsizei indexCount)
    {
        if (_ibo) glDeleteBuffers(1, &_ibo);

        _index_type = indexType;
        _index_count = indexCount;

        glBindVertexArrayOES(_vao);
        
        glGenBuffers(1, &_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, OpenGL::SizeofTypeByEnum(indexType) * indexCount, data, indexUsage);
    }
    
	void AddPackedVertices(GLenum vertexUsage, GLenum primitiveMode, VertexDescription vertexDescription, GLvoid *data, GLsizei vertexCount)
    {
        if (_vbo) glDeleteBuffers(1, &_vbo);

        _primitive_mode = primitiveMode;
        _vertex_count = vertexCount;
        _vertex_size = vertexDescription.VertexSize();
        glBindVertexArrayOES(_vao);

        glGenBuffers(1, &_vbo);	
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexDescription.VertexSize() * vertexCount, data, vertexUsage);

        for (unsigned int i = 0; i < vertexDescription.AttributeCount(); ++i)
        {
            VertexAttribute attribute = vertexDescription[i];

            glVertexAttribPointer(i, attribute.component_count, attribute.component_type, attribute.normalized, _vertex_size, vertexDescription.AttributeOffset(i));
            glEnableVertexAttribArray(i);
        }
    }
    
	GLvoid* BeginMapIndices(GLenum access = GL_WRITE_ONLY_OES)
    {   
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        if (access == GL_WRITE_ONLY_OES) glBufferData(GL_ELEMENT_ARRAY_BUFFER, _index_count * OpenGL::SizeofTypeByEnum(_index_type), NULL, GL_DYNAMIC_DRAW);
        return glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, access);
    }

    GLvoid* BeginMapVertices(GLenum access = GL_WRITE_ONLY_OES)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        if (access == GL_WRITE_ONLY_OES) glBufferData(GL_ARRAY_BUFFER, _vertex_count * _vertex_size, NULL, GL_DYNAMIC_DRAW);
        return glMapBufferOES(GL_ARRAY_BUFFER, access);
    }

    void Draw()
    {
        glBindVertexArrayOES(_vao);
        
        if (_ibo)
        {
            glDrawElements(_primitive_mode, _index_count, _index_type, 0);
        }
        else
        {
            glDrawArrays(_primitive_mode, 0, _vertex_count);
        }
    }

    void DrawIndexed()
    {
        glBindVertexArrayOES(_vao);
        glDrawElements(_primitive_mode, _index_count, _index_type, 0);
    }

    void DrawRaw()
    {
        glBindVertexArrayOES(_vao);
        glDrawArrays(_primitive_mode, 0, _vertex_count);
    }

    void EndMapIndices()
    {
        glUnmapBufferOES(GL_ELEMENT_ARRAY_BUFFER);
    }
    
    void EndMapVertices()
    {
        glUnmapBufferOES(GL_ARRAY_BUFFER);
    }
    
    GLuint IndexBufferID()
    {
        return _ibo;
    }

    GLuint VertexArrayID()
    {
        return _vao;
    }

    GLuint VertexBufferID()
    {
        return _vbo;
    }
};

#endif
