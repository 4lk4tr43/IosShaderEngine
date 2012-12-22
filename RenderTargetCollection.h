#ifndef RenderTargetCollection_h__
#define RenderTargetCollection_h__ RenderTargetCollection_h__

#include <vector>
#include <string>
#include <OpenGLES/ES2/gl.h>

#include "OpenGLUtility.h"

namespace Eos
{
    namespace OpenGL
    {
        class RenderTargetCollection
        {
            GLuint _fboID, _depthTextureID, _currentColorTextureID;
            GLsizei _fboWidth, _fboHeight;
            GLenum _depthTextureDataFormat;
            vector<GLuint> _colorTextureIDs;
            vector<GLenum> _colorTextureFormats;
            
            // GL_OES_depth_texture must be supported
            static GLuint CreateDepthTexture(GLenum pixelDataType, GLsizei width, GLsizei height) 
            {
                GLuint depthTextureID;
                
                glGenTextures(1, &depthTextureID);        
                glBindTexture(GL_TEXTURE_2D, depthTextureID);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, pixelDataType, NULL);
                
                return depthTextureID;
            }
            static GLuint CreateColorTexture(GLenum internalFormat, GLenum pixelFormat, GLenum pixelDataType, GLsizei width, GLsizei height)
            {
                GLuint textureID;
                
                glGenTextures(1, &textureID);        
                glBindTexture(GL_TEXTURE_2D, textureID);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, pixelFormat, pixelDataType, NULL);
                
                return textureID;
            }
            
        public:
            
            RenderTargetCollection(GLsizei width, GLsizei height, GLenum depthTextureDataFormat = GL_NONE, string *errorLog = nullptr)
            {   
                _fboID = 0;
                _depthTextureID = 0;
                
                _fboWidth = width;
                _fboHeight = height;            
                
                glGenFramebuffers(1, &_fboID);
                
                if (depthTextureDataFormat == GL_NONE) return;
                
                SetDepthTextureDataFormat(depthTextureDataFormat);
                            
                GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (status != GL_FRAMEBUFFER_COMPLETE && errorLog != 0)
                {
                    *errorLog = *errorLog + string("RenderTargetCollection: frame buffer incomplete, %x\n", status);  
                }
            }
            ~RenderTargetCollection()
            {
                for (unsigned int i = 0; _colorTextureIDs.size(); ++i) glDeleteTextures(1, &_colorTextureIDs[i]);
                if (_depthTextureID) glDeleteRenderbuffers(1, &_depthTextureID);
                if (_fboID) glDeleteFramebuffers(1, &_fboID);
            }
            
            void SetDepthTextureDataFormat(GLenum depthTextureDataFormat)
            {
                if (_depthTextureID) glDeleteTextures(1, &_depthTextureID);
                
                _depthTextureDataFormat = depthTextureDataFormat;
                glBindFramebuffer(GL_FRAMEBUFFER, _fboID);
                
                _depthTextureID = RenderTargetCollection::CreateDepthTexture(_depthTextureDataFormat, _fboWidth, _fboHeight);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTextureID, 0);
            }
            void AddTextureTarget(GLenum internalFormat, GLenum pixelFormat, GLenum pixelDataType)
            {
                _colorTextureIDs.push_back(RenderTargetCollection::CreateColorTexture(internalFormat, pixelFormat, pixelDataType, _fboWidth, _fboHeight));            
                
                _colorTextureFormats.push_back(internalFormat);
                _colorTextureFormats.push_back(pixelFormat);
                _colorTextureFormats.push_back(pixelDataType);
            }
            GLvoid RemoveTextureTarget(unsigned int targetNumber)
            {            
                glDeleteTextures(1, &_colorTextureFormats[targetNumber]);
                
                _colorTextureIDs[targetNumber] = 0;
                _colorTextureFormats[targetNumber*3] = GL_NONE;
                _colorTextureFormats[targetNumber*3 + 1] = GL_NONE;
                _colorTextureFormats[targetNumber*3 + 2] = GL_NONE;
                
                vector<GLuint> newTextureIDs;            
                for (unsigned int i = 0; i < _colorTextureIDs.size(); ++i) 
                {
                    GLuint texture = _colorTextureIDs[i];
                    if (texture != 0) newTextureIDs.push_back(texture);        
                }               
                _colorTextureIDs = newTextureIDs;
                
                vector<GLenum> newTextureFormats;
                for (unsigned int i = 0; i < _colorTextureFormats.size(); ++i) 
                {
                    GLenum format = _colorTextureFormats[i];
                    if (format != GL_NONE) newTextureFormats.push_back(format);                      
                } 
                _colorTextureFormats = newTextureFormats;
            }
            
            void ActivateRendertarget(GLint targetNumber, GLint attachmentSlot = 0, GLint x = 0, GLint y = 0, GLsizei width = 0, GLsizei height = 0)
            {            
                glBindFramebuffer(GL_FRAMEBUFFER, _fboID);
                _currentColorTextureID = _colorTextureIDs[targetNumber];
                
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentSlot, GL_TEXTURE_2D, _currentColorTextureID, 0);
                
                GLsizei w = width == 0 ? _fboWidth : width;
                GLsizei h = height == 0 ? _fboHeight : height;
                glViewport(x, y, w, h);
            }        
            void ActivateDepthtargetOnly(GLint x = 0, GLint y = 0, GLsizei width = 0, GLsizei height = 0)
            {            
                glBindFramebuffer(GL_FRAMEBUFFER, _fboID); 
                
                GLsizei w = width == 0 ? _fboWidth : width;
                GLsizei h = height == 0 ? _fboHeight : height;
                glViewport(x, y, w, h);
            }
            
            void BindTargetToSampler(GLint targetNumber, GLint samplerNumber)
            {
                OpenGLUtility::ActivateTextureUnit(samplerNumber);            
                glBindTexture(GL_TEXTURE_2D, _colorTextureIDs[targetNumber]);
            }        
            void BindDepthToSampler(GLint samplerNumber)
            {
                OpenGLUtility::ActivateTextureUnit(samplerNumber);            
                glBindTexture(GL_TEXTURE_2D, _depthTextureID);
            }
        };
    }
}

#endif // RenderTargetCollection_h__
