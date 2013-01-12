#ifndef post_shader_h__
#define post_shader_h__

#include "shader.h"
#include "vertex_array_object.h"

#define FULLSCREEN_QUAD_VERTEX_SHADER (GLchar*)"attribute vec2 position;attribute vec2 uv;varying vec2 uv_var;void main(){gl_Position=vec4(position,0.0,1.0);uv_var=uv;}"
#define FULLSCREEN_QUAD_VERTEX_SHADER_ATTRIB_NAMES (GLchar*)"position,uv"

static unsigned int _post_shader_reference_count = 0;
static GLuint _post_shader_quad_vertex_shader_id = 0, _post_shader_vao_id = 0, _post_shader_vbo_id = 0;

class PostShader : public Shader
{
    static void LazyInitVertexStage()
    {
        if (!_post_shader_vao_id)
        {
            const GLfloat vertexBuffer[] =
            {
                1.0f, 1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f, 0.0f
            };
  
            GLsizei vertexSize = sizeof(GLfloat) * 4;        
            glGenVertexArraysOES(1, &_post_shader_vao_id);
            glBindVertexArrayOES(_post_shader_vao_id);           
            glGenBuffers(1, &_post_shader_vbo_id);	
            glBindBuffer(GL_ARRAY_BUFFER, _post_shader_vbo_id);
            glBufferData(GL_ARRAY_BUFFER,  vertexSize * 4, vertexBuffer, GL_STATIC_DRAW);     
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertexSize, 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid *)8) ;
            glEnableVertexAttribArray(1);			                                  
            _post_shader_quad_vertex_shader_id = Shader::CompileShaderFromString(GL_VERTEX_SHADER, FULLSCREEN_QUAD_VERTEX_SHADER);
        }
    }

public:
    PostShader(GLuint fragment_shader_id, GLchar *uniform_names_separated_by_comma = nullptr, string *error_log = nullptr)
    {        
		_uniforms = nullptr;  
        LazyInitVertexStage();                
        ++_post_shader_reference_count;
		_program_id = Shader::LoadShader(FULLSCREEN_QUAD_VERTEX_SHADER_ATTRIB_NAMES, _post_shader_quad_vertex_shader_id, fragment_shader_id, GL_FALSE, error_log);
        if (uniform_names_separated_by_comma) 
			_uniforms = Shader::GetUniformLocations(string(uniform_names_separated_by_comma), _program_id);
    }

    PostShader(GLchar *fragment_shader_string, GLchar *uniform_names_separated_by_comma = nullptr, string *error_log = nullptr)
    {        
		_uniforms = nullptr;  
        LazyInitVertexStage();                
        ++_post_shader_reference_count;
		GLuint fragment_shader_id = Shader::CompileShaderFromString(GL_FRAGMENT_SHADER, fragment_shader_string, error_log);
        _program_id = Shader::LoadShader(FULLSCREEN_QUAD_VERTEX_SHADER_ATTRIB_NAMES, _post_shader_quad_vertex_shader_id, fragment_shader_id, GL_TRUE, error_log);  
        if (uniform_names_separated_by_comma) 
			_uniforms = Shader::GetUniformLocations(string(uniform_names_separated_by_comma), _program_id);
    }

    ~PostShader()
    {
		if (_program_id) 
		{
			glDeleteProgram(_program_id);
			_program_id = 0;
		}
		if (_uniforms)
		{
			delete[] _uniforms;
			_uniforms = nullptr;
		}

        --_post_shader_reference_count;
        if (_post_shader_reference_count == 0)
        {
            if (_post_shader_quad_vertex_shader_id) 
            { 
                glDeleteShader(_post_shader_quad_vertex_shader_id);
                _post_shader_quad_vertex_shader_id = 0;
            }
            if (_post_shader_vbo_id) 
            {
                glDeleteBuffers(1, &_post_shader_vbo_id);
                _post_shader_vbo_id = 0;
            }
            if (_post_shader_vao_id)
            {
                glDeleteVertexArraysOES(1, &_post_shader_vao_id);
                _post_shader_vao_id = 0;
            }
        }
    }

    void Draw()
    {
		Activate();
        glBindVertexArrayOES(_post_shader_vao_id); 
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);            
    }
};

#endif
