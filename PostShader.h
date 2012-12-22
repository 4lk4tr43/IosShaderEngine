#ifndef PostShader_h__
#define PostShader_h__ PostShader_h__

#include "Shader.h"

#define FULLSCREEN_QUAD_VERTEX_SHADER (GLchar*)"attribute vec2 position;attribute vec2 uv;varying vec2 uvVarying;void main(){gl_Position=vec4(position,0.0,1.0);uvVarying=uv;}"
#define FULLSCREEN_QUAD_VERTEX_SHADER_ATTRIB_NAMES (GLchar*)"position,uv"

namespace Eos
{
    namespace OpenGL
    {
        static unsigned int _postShaderReferenceCount;
        static GLuint _postShaderQuadVertexShaderID;
        static GLuint vao = 0, vbo = 0;
        
        class PostShader : public Shader
        {		
            static void InitializeVertexStageOnDemand()
            {
                if (!vao)
                {
                    const GLfloat vertexBuffer[] =
                    {
                        1.0f, 1.0f, 1.0f, 1.0f,
                        -1.0f, 1.0f, 0.0f, 1.0f,
                        1.0f, -1.0f, 1.0f, 0.0f,
                        -1.0f, -1.0f, 0.0f, 0.0f
                    };                
                   
                    GLsizei vertexSize = sizeof(GLfloat) * 4;
                    
                    glGenVertexArraysOES(1, &vao);
                    glBindVertexArrayOES(vao);
                    
                    glGenBuffers(1, &vbo);	
                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    glBufferData(GL_ARRAY_BUFFER,  vertexSize * 4, vertexBuffer, GL_STATIC_DRAW);
                    
                    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertexSize, 0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid *)8) ;
                    glEnableVertexAttribArray(1);			
                                                   
                    _postShaderQuadVertexShaderID = Shader::CompileShaderFromString(GL_VERTEX_SHADER, FULLSCREEN_QUAD_VERTEX_SHADER);
                }
            }

        public:
            PostShader(GLuint fragmentShaderID, GLchar * uniformNamesSeperatedByComma = nullptr, string *errorLog = nullptr)
            {
                _postShaderReferenceCount++;
                InitializeVertexStageOnDemand();
                
                _programID = Shader::LoadShader(FULLSCREEN_QUAD_VERTEX_SHADER_ATTRIB_NAMES, _postShaderQuadVertexShaderID, fragmentShaderID, GL_FALSE, errorLog);
                if (uniformNamesSeperatedByComma) _uniforms = Shader::GetUniformLocations(string(uniformNamesSeperatedByComma), _programID);
            }
            PostShader(GLchar * fragmentShaderString, GLchar * uniformNamesSeperatedByComma = nullptr, string *errorLog = nullptr)
            {
                _postShaderReferenceCount++;
                InitializeVertexStageOnDemand();
                
                GLuint fragmentShaderID = Shader::CompileShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderString);
                _programID = Shader::LoadShader(FULLSCREEN_QUAD_VERTEX_SHADER_ATTRIB_NAMES, _postShaderQuadVertexShaderID, fragmentShaderID, GL_TRUE, errorLog);
                
                if (uniformNamesSeperatedByComma) _uniforms = Shader::GetUniformLocations(string(uniformNamesSeperatedByComma), _programID);
            }
            ~PostShader()
            {
                _postShaderReferenceCount--;
                if (_postShaderReferenceCount == 0)
                {
                    if (_postShaderQuadVertexShaderID) 
                    { 
                        glDeleteShader(_postShaderQuadVertexShaderID);
                        _postShaderQuadVertexShaderID = 0;
                    }
                    if (vbo) 
                    {
                        glDeleteBuffers(1, &vbo);
                        vbo = 0;
                    }
                    if (vao)
                    {
                        glDeleteVertexArraysOES(1, &vao);
                        vao = 0;
                    }
                }
            }

            void Draw()
            {
                glBindVertexArrayOES(vao); 
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);            
            }
        };
    }
}

#endif // PostShader_h__
