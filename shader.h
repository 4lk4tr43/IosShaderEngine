#ifndef shader_h__
#define shader_h__

#include "opengl.h"

class Shader
{
protected:
    GLint *_uniforms;
	GLuint _program_id;

    Shader() {}            
            
    static GLint * GetUniformLocations(string names_separated_by_comma, GLuint program)
    {
        StringTokenizer tokens = StringTokenizer(names_separated_by_comma, ',');
        GLint *locations = new GLint[tokens.LineCount()];
                
        for (unsigned int i = 0; i < tokens.LineCount(); ++i) locations[i] = glGetUniformLocation(program, tokens[i].c_str());
                
        return locations;
    }
            
    static GLboolean LinkProgram(GLuint program_reference, string *error_log = nullptr)
    {
        glLinkProgram(program_reference);
                
        GLint status;
        glGetProgramiv(program_reference, GL_LINK_STATUS, &status);
        if (!status)
        {
            if (error_log)
            {
                GLsizei log_length;
                glGetProgramiv(program_reference, GL_INFO_LOG_LENGTH, &log_length);
                if (log_length > 0)
                {
                    GLchar *log = new GLchar[log_length];
                    glGetProgramInfoLog(program_reference, log_length, &log_length, log);
                    *error_log = *error_log + string(log);
                    delete[] log;
                }
            }               
            return GL_FALSE;
        }
        return GL_TRUE;
    }

	static GLuint LoadShader(string attrib_names_separated_by_comma, GLuint vertex_shader_id, GLuint fragment_shader_id, GLboolean delete_shaders_after_load = GL_TRUE, string *error_log = nullptr)
	{
		GLuint program = glCreateProgram();
		StringTokenizer attrib_names = StringTokenizer(attrib_names_separated_by_comma, ',');
		glAttachShader(program, vertex_shader_id);
		glAttachShader(program, fragment_shader_id);
		for (GLuint i = 0; i < attrib_names.LineCount(); ++i) 
			glBindAttribLocation(program, i, attrib_names[i].c_str());
		if (!Shader::LinkProgram(program, error_log))
		{
			glDeleteProgram(program);
			program = 0;
		}
		if (delete_shaders_after_load)
		{
			if (vertex_shader_id) glDeleteShader(vertex_shader_id);
			if (fragment_shader_id) glDeleteShader(fragment_shader_id);
		}       
		return program;
	}

public:
	Shader(GLuint vertex_shader_id, GLuint fragment_Shader_id, string attrib_names_separated_by_comma, string uniform_names_separated_by_comma = "", string *errorLog = nullptr)
    {
        _uniforms = nullptr;
        _program_id = Shader::LoadShader(attrib_names_separated_by_comma, vertex_shader_id, fragment_Shader_id, GL_FALSE, errorLog);
        if (uniform_names_separated_by_comma.length())
			_uniforms = Shader::GetUniformLocations(uniform_names_separated_by_comma, _program_id);
    }			
    
	Shader(GLchar *vertex_shader_string, GLchar *fragment_shader_string, string attrib_names_separated_by_comma, string uniform_names_separated_by_comma = "", string *error_log = nullptr)
    {            
        _uniforms = nullptr;    
        if (error_log)
        {                
            GLuint vertex_shader_id = Shader::CompileShaderFromString(GL_VERTEX_SHADER, vertex_shader_string, error_log);
            GLuint fragment_shader_id = Shader::CompileShaderFromString(GL_FRAGMENT_SHADER, fragment_shader_string, error_log);
            _program_id = Shader::LoadShader(attrib_names_separated_by_comma, vertex_shader_id, fragment_shader_id, GL_TRUE, error_log);
        }
        else
            _program_id = Shader::LoadShader(attrib_names_separated_by_comma, Shader::CompileShaderFromString(GL_VERTEX_SHADER, vertex_shader_string), Shader::CompileShaderFromString(GL_FRAGMENT_SHADER, fragment_shader_string), GL_TRUE);

        if (uniform_names_separated_by_comma.length())
			_uniforms = Shader::GetUniformLocations(uniform_names_separated_by_comma, _program_id);
    }

    ~Shader()
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
    }

    void Activate()
    {
        glUseProgram(_program_id);
    }

	static GLuint CompileShaderFromString(GLenum type, const GLchar *shader_string, string *error_log = nullptr)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &shader_string, 0);
		glCompileShader(shader);
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			if (error_log)
			{
				GLsizei log_length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
				if (log_length > 0)
				{
					GLchar *log = new GLchar[log_length];
					glGetShaderInfoLog(shader, log_length, &log_length, log);
					*error_log = *error_log + string(log);
					delete[] log;
				}
			}
			if (shader) 
				glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

    GLint UniformID(unsigned int uniform_position)
    {
        return _uniforms[uniform_position];
    }
};

#endif
