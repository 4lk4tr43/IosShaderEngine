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
    GLuint _ibo, _vao;
	vector<GLuint> _vbos;

	void PrepareVao( GLenum primitive_mode, GLsizei vertex_count, VertexDescription &vertex_description ) 
	{
		if (_vbos.size())
		{
			glDeleteBuffers(_vbos.size(), &_vbos[0]);
			_vbos.clear();
		}
		_primitive_mode = primitive_mode;
		_vertex_count = vertex_count;
		_vertex_size = vertex_description.Size();
		glBindVertexArrayOES(_vao);	
	}

public:
    VertexArrayObject()
    {        
        _ibo = 0;
        glGenVertexArraysOES(1, &_vao);
    }
    
    ~VertexArrayObject() 
    {
        if (_ibo) 
			glDeleteBuffers(1, &_ibo);
		if (_vbos.size())
		{
			glDeleteBuffers(_vbos.size(), &_vbos[0]);
			_vbos.clear();
		}
        if (_vao) 
			glDeleteVertexArraysOES(1, &_vao);
    }
    
    void AddIndices(GLenum indexUsage, GLenum index_type, GLvoid *data, GLsizei index_count)
    {
        if (_ibo)
			glDeleteBuffers(1, &_ibo);
        _index_type = index_type;
        _index_count = index_count;
        glBindVertexArrayOES(_vao);        
        glGenBuffers(1, &_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, OpenGL::SizeofTypeByEnum(index_type) * index_count, data, indexUsage);
    }
    
	void AddPackedVertices(GLenum vertex_usage, GLenum primitive_mode, VertexDescription &vertex_description, GLvoid *data, GLsizei vertex_count)
    {
        PrepareVao(primitive_mode, vertex_count, vertex_description);
		GLuint vbo;
        glGenBuffers(1, &vbo);
		_vbos.push_back(vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex_description.Size() * vertex_count, data, vertex_usage);
        for (GLsizei i = 0; i < vertex_description.AttributeCount(); ++i)
        {
            VertexAttribute attribute = vertex_description[i];
            glVertexAttribPointer(i, attribute.component_count, attribute.component_type, attribute.normalized, _vertex_size, vertex_description.AttributeOffset(i));
            glEnableVertexAttribArray(i);
        }
    }

	void AddVertices(GLenum vertex_usage, GLenum primitive_mode, VertexDescription &vertex_description, vector<GLvoid*> &data_buffers, GLsizei vertex_count)
	{
		(size_t)vertex_description.AttributeCount() < data_buffers.size() ? (size_t)vertex_description.AttributeCount() : data_buffers.size();
		PrepareVao(primitive_mode, vertex_count, vertex_description);
		auto min_attributes_with_data = (GLsizei)((size_t)vertex_description.AttributeCount() < data_buffers.size() ? 
			(size_t)vertex_description.AttributeCount() : data_buffers.size());
		for (GLsizei i = 0; i < min_attributes_with_data; ++i)
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			_vbos.push_back(vbo);
			VertexAttribute attribute = vertex_description[i];
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, attribute.Size() * vertex_count, data_buffers[i], vertex_usage);
			glVertexAttribPointer(i, attribute.component_count, attribute.component_type, attribute.normalized, 0, 0);
			glEnableVertexAttribArray(i);
		}
	}

	void AddVertices(vector<GLenum> &vertex_usages, GLenum primitive_mode, VertexDescription &vertex_description, vector<GLvoid*> &data_buffers, GLsizei vertex_count)
	{
		PrepareVao(primitive_mode, vertex_count, vertex_description);
		auto min_attributes_with_data = (GLsizei)((size_t)vertex_description.AttributeCount() < data_buffers.size() ?
			(size_t)vertex_description.AttributeCount() : data_buffers.size());
		for (GLsizei i = 0; i < min_attributes_with_data; ++i)
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			_vbos.push_back(vbo);
			VertexAttribute attribute = vertex_description[i];
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, attribute.Size() * vertex_count, data_buffers[i], vertex_usages[i]);
			glVertexAttribPointer(i, attribute.component_count, attribute.component_type, attribute.normalized, 0, 0);
			glEnableVertexAttribArray(i);
		}
	}

	GLvoid* BeginMapIndices(GLenum access = GL_WRITE_ONLY_OES)
    {   
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        if (access == GL_WRITE_ONLY_OES) 
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _index_count * OpenGL::SizeofTypeByEnum(_index_type), NULL, GL_DYNAMIC_DRAW);
        return glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, access);
    }

    GLvoid* BeginMapVertices(GLenum access = GL_WRITE_ONLY_OES, unsigned int vbo_index = 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbos[vbo_index]);
        if (access == GL_WRITE_ONLY_OES) 
			glBufferData(GL_ARRAY_BUFFER, _vertex_count * _vertex_size, NULL, GL_DYNAMIC_DRAW);
        return glMapBufferOES(GL_ARRAY_BUFFER, access);
    }

    void Draw()
    {
        if (_ibo)
			DrawIndexed();
		else
			DrawRaw();
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
};

#endif
