#ifndef opengl_h__
#define opengl_h__

#include <iostream>

#ifdef WIN32
	#define GLEW_STATIC 
	#include "gl/glew.h"
	#include "gl/glfw.h"
	typedef __int32 GLfixed;
#else
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
#endif 

#include "string_tokenizer.h"

// An OpenGL context is needed to execute these methods, else the program will crash with an access violation
class OpenGL
{    
public:    
    static void ActivateTextureUnit(GLint textureUnit)
    {
		glActiveTexture(GL_TEXTURE0 + textureUnit);
    }

    static char* AllExtensions()
    {
        return (char*)glGetString(GL_EXTENSIONS);
    }
    
    static void AllExtensionsToConsole()
    {
        cout << OpenGL::AllExtensions() << endl;
    }

    static bool AreExtensionsSupported(GLchar *extensions_seperated_by_comma)
    {
        string extensions(OpenGL::AllExtensions());
        StringTokenizer tokens(string(extensions_seperated_by_comma), string(","));
        for (unsigned int i = 0; i < tokens.LineCount(); ++i)
        {
            if (extensions.find(tokens[i]) == string::npos)
                return false;
        }
        return true;
    }
    
    static void ErrorToConsole()
    {
        cout << "OpenGL Error: " << glGetError() << endl;
    }
    
    static bool IsExtensionSupported(GLchar *extension)
    {
        string extension_to_find(extension);
        string extensions(OpenGL::AllExtensions());
        return extensions.find(extension_to_find) != string::npos;
    }
    
    static GLsizei SizeofTypeByEnum(GLenum type_enum)
    {
        switch (type_enum)
        {
            case GL_BYTE:
                return sizeof(GLbyte);
                
            case GL_UNSIGNED_BYTE:
                return sizeof(GLubyte);
                
            case GL_SHORT:
                return sizeof(GLshort);
                
            case GL_UNSIGNED_SHORT:
                return sizeof(GLushort);
                
            case GL_INT:
                return sizeof(GLint);
                
            case GL_UNSIGNED_INT:
                return sizeof(GLuint);
                
            case GL_FLOAT:
                return sizeof(GLfloat);
                
            case GL_FIXED:
                return sizeof(GLfixed);
        }
    }
};

#endif

