#ifndef opengl_h__
#define opengl_h__

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "variable.h"
        
        // An OpenGL context is needed to execute these methods, else the program will crash with an access violation
        class OpenGLUtility
        {
            static GLint _current_texture_unit = -1;
        public:
            
            static void activate_texture_unit(GLint textureUnit)
            {
                if (_current_texture_unit != textureUnit)
                {					
                    glActiveTexture(GL_TEXTURE0 + textureUnit);
                    _current_texture_unit = textureUnit;
                }
            }
            
            static GLsizei SizeofTypeByEnum(GLenum type)
            {
                GLsizei typeSize = 0;

                switch (type)
                {
                case GL_BYTE:

                    typeSize = sizeof(GLbyte);
                    break;

                case GL_UNSIGNED_BYTE:

                    typeSize = sizeof(GLubyte);
                    break;

                case GL_SHORT:

                    typeSize = sizeof(GLshort);
                    break;

                case GL_UNSIGNED_SHORT:

                    typeSize = sizeof(GLushort);
                    break;

                case GL_INT:

                    typeSize = sizeof(GLint);
                    break;

                case GL_UNSIGNED_INT:

                    typeSize = sizeof(GLuint);
                    break;

                case GL_FLOAT:

                    typeSize = sizeof(GLfloat);
                    break;

                case GL_FIXED:

                    typeSize = sizeof(GLfixed);
                    break;
                }

                return typeSize;
            }

            static char* AllExtensions()
            {
                return (char*)glGetString(GL_EXTENSIONS);
            }
            
            static void AllExtensionsToConsole()
            {
                printf("%s \n", OpenGLUtility::AllExtensions());
            }
            
            static bool SupportedExtension(GLchar *extension)
            {
                string extensionToFind(extension);
                string extensions(OpenGLUtility::AllExtensions());

                if (extensions.find(extensionToFind) == string::npos)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            
            static bool SupportedExtensions(GLchar *extensionsSeperatedByComma)
            {            
                string extensions(OpenGLUtility::AllExtensions());
                
                StringTokenizer tokens(string(extensionsSeperatedByComma), string(","));
                for (unsigned int i = 0; i < tokens.LineCount(); ++i) 
                {
                    if (extensions.find(tokens[i]) == string::npos) return false;
                }
                
                return true;
            }

            static void ErrorToConsole()
            {
                printf("OpenGL Error: 0x%04X \n", glGetError());
            }
        };

#endif

