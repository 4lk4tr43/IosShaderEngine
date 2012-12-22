#ifndef Shader_h__
#define Shader_h__ Shader_h__

// FUTURE VERSION (OpenGL 3.0 +):
// It is used for MRT, but the GLSL location definition is stronger than this.
// This function must be used before program linking (done in LoadShader)
//
// static void SetFragmentOutOrder(string namesSeperatedByComma, GLuint program)
// {
//     StringTokenizer tokens = StringTokenizer(namesSeperatedByComma, string(","));
//     for (unsigned int i = 0; i < tokens.LineCount(); ++i) 
//     {
//         glBindFragDataLocation(program, i, tokens[i].c_str());
//     }
// }

#include <OpenGLES/ES2/gl.h>

#include "../Helpers/StringTokenizer.h"

using namespace Eos::Helpers;

namespace Eos
{
    namespace OpenGL
    {
        class Shader
        {
        protected:
            GLuint _programID;
            GLint *_uniforms;

            Shader() {}            
            
            static GLboolean LinkProgram(GLuint programReference, string *errorLog = nullptr)
            {
                glLinkProgram(programReference);
                
                GLint status;
                glGetProgramiv(programReference, GL_LINK_STATUS, &status);
                if (status == 0)
                {
                    if (errorLog)
                    {
                        GLsizei logLength;
                        glGetProgramiv(programReference, GL_INFO_LOG_LENGTH, &logLength);
                        if (logLength > 0)
                        {
                            GLchar *log = new GLchar[logLength];
                            glGetProgramInfoLog(programReference, logLength, &logLength, log);
                            *errorLog = *errorLog + string(log);
                            delete[] log;
                        }
                    }
                    
                    return GL_FALSE;
                }
                
                return GL_TRUE;
            }
			static GLuint LoadShader(string attribNamesSeperatedByComma, GLuint vertexShaderID, GLuint fragmentShaderID, GLboolean deleteShadersAfterLoad = GL_TRUE, string *errorLog = nullptr)
			{
				GLuint program = glCreateProgram();
				StringTokenizer attribNames = StringTokenizer(attribNamesSeperatedByComma, string(","));
                
				glAttachShader(program, vertexShaderID);
				glAttachShader(program, fragmentShaderID);
                
				for (GLuint i = 0; i < attribNames.LineCount(); ++i) glBindAttribLocation(program, i, attribNames[i].c_str());
                
				if (!Shader::LinkProgram(program, errorLog))
				{
					glDeleteProgram(program);
					program = 0;
				}
                
				if (deleteShadersAfterLoad)
				{
					if (vertexShaderID) glDeleteShader(vertexShaderID);
					if (fragmentShaderID) glDeleteShader(fragmentShaderID);
				}
                
				return program;
			}
            static GLint * GetUniformLocations(string namesSeperatedByComma, GLuint program)
            {
                StringTokenizer tokens = StringTokenizer(namesSeperatedByComma, string(","));
                GLint *locations = new GLint[tokens.LineCount()];
                
                for (unsigned int i = 0; i < tokens.LineCount(); ++i) locations[i] = glGetUniformLocation(program, tokens[i].c_str());
                
                return locations;
            }
            
        public:
            static GLuint CompileShaderFromString(GLenum type, const GLchar *shaderString, string *errorLog = nullptr)
            {
                GLuint shader = glCreateShader(type);
                glShaderSource(shader, 1, &shaderString, 0);
                glCompileShader(shader);
                
                GLint status;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
                if (status == 0)
                {
                    if (errorLog)
                    {
                        GLsizei logLength;
                        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
                        if (logLength > 0)
                        {
                            GLchar *log = new GLchar[logLength];
                            glGetShaderInfoLog(shader, logLength, &logLength, log);
                            *errorLog = *errorLog + string(log);
                            delete[] log;
                        }
                    }
                    
                    if (shader) glDeleteShader(shader);
                    return 0;
                }
                
                return shader;
            }
            Shader(GLuint vertexShaderID, GLuint fragmentShaderID, GLchar *attribNamesSeperatedByComma, GLchar *uniformNamesSeperatedByComma = nullptr, string *errorLog = nullptr)
            {
                _uniforms = nullptr;
                
                _programID = Shader::LoadShader(attribNamesSeperatedByComma, vertexShaderID, fragmentShaderID, GL_FALSE, errorLog);
                if (uniformNamesSeperatedByComma) _uniforms = Shader::GetUniformLocations(string(uniformNamesSeperatedByComma), _programID);
            }			
            Shader(GLchar *vertexShaderString, GLchar *fragmentShaderString, GLchar *attribNamesSeperatedByComma, GLchar *uniformNamesSeperatedByComma = nullptr, string *errorLog = nullptr)
            {            
                _uniforms = 0;
                
                if (errorLog)
                {                
                    *errorLog = *errorLog + string("Vertex Shader Compile:\n");
                    GLuint vertexShaderID = Shader::CompileShaderFromString(GL_VERTEX_SHADER, vertexShaderString, errorLog);
                    *errorLog = *errorLog + string("Fragment Shader Compile:\n");
                    GLuint fragmentShaderID = Shader::CompileShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderString, errorLog);
                    *errorLog = *errorLog + string("Shader Program Link:\n");
                    _programID = Shader::LoadShader(attribNamesSeperatedByComma, vertexShaderID, fragmentShaderID, GL_TRUE, errorLog);
                }
                else
                {
                    _programID = Shader::LoadShader(attribNamesSeperatedByComma, Shader::CompileShaderFromString(GL_VERTEX_SHADER, vertexShaderString), Shader::CompileShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderString), GL_TRUE);
                }

                if (uniformNamesSeperatedByComma) _uniforms = Shader::GetUniformLocations(string(uniformNamesSeperatedByComma), _programID);
            }
            ~Shader()
            {
                if (_programID) glDeleteProgram(_programID);
                if (_uniforms) delete[] _uniforms;
            }

            GLint UniformID(unsigned int uniformPosition)
            {
                return _uniforms[uniformPosition];
            }
            void Activate()
            {
                glUseProgram(_programID);
            }
        };
    }
}

#endif // Shader_h__
