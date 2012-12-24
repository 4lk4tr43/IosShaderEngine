#ifndef render_target_collection_h__
#define render_target_collection_h__

#include "opengl.h"

class RenderTargetCollection
{
    GLenum _depth_texture_data_format;
    GLsizei _fbo_width, _fbo_height;
	GLuint _fbo_id, _depth_texture_id, _current_color_texture_id;
    vector<GLenum> _color_texture_formats;
	vector<GLuint> _color_texture_ids;
            
    // GL_OES_depth_texture must be supported
    static GLuint CreateDepthTexture(GLenum pixel_data_type, GLsizei width, GLsizei height) 
    {
        GLuint depth_texture_id;                
        glGenTextures(1, &depth_texture_id);        
        glBindTexture(GL_TEXTURE_2D, depth_texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, pixel_data_type, NULL);
        return depth_texture_id;
    }

    static GLuint CreateColorTexture(GLenum internal_format, GLenum pixel_format, GLenum pixel_data_type, GLsizei width, GLsizei height)
    {
        GLuint texture_id;
        glGenTextures(1, &texture_id);        
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, pixel_format, pixel_data_type, NULL);
        return texture_id;
    }
            
public:            
    RenderTargetCollection(GLsizei width, GLsizei height, GLenum depth_texture_data_format = GL_NONE, string *error_log = nullptr)
    {          
        _depth_texture_id = 0;        
        _fbo_id = 0;
		_fbo_width = width;
        _fbo_height = height;            
        glGenFramebuffers(1, &_fbo_id);
        if (depth_texture_data_format == GL_NONE)
			return;    
        SetDepthTextureDataFormat(depth_texture_data_format);         
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE && error_log != 0)
            *error_log = *error_log + string("RenderTargetCollection: frame buffer incomplete, %x\n", status);
    }

    ~RenderTargetCollection()
    {
        for (unsigned int i = 0; _color_texture_ids.size(); ++i)
			glDeleteTextures(1, &_color_texture_ids[i]);
        if (_depth_texture_id)
			glDeleteRenderbuffers(1, &_depth_texture_id);
        if (_fbo_id)
			glDeleteFramebuffers(1, &_fbo_id);
    }
            
    void ActivateDepthtargetOnly(GLint x = 0, GLint y = 0, GLsizei width = 0, GLsizei height = 0)
    {            
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id); 
        GLsizei w = width == 0 ? _fbo_width : width;
        GLsizei h = height == 0 ? _fbo_height : height;
        glViewport(x, y, w, h);
    }
            
    void ActivateRendertarget(GLint target_number, GLint attachment_slot = 0, GLint x = 0, GLint y = 0, GLsizei width = 0, GLsizei height = 0)
    {            
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);
        _current_color_texture_id = _color_texture_ids[target_number];            
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment_slot, GL_TEXTURE_2D, _current_color_texture_id, 0);  
        GLsizei w = width == 0 ? _fbo_width : width;
        GLsizei h = height == 0 ? _fbo_height : height;
        glViewport(x, y, w, h);
    }

    void AddTextureTarget(GLenum internal_format, GLenum pixel_format, GLenum pixel_data_type)
    {
        _color_texture_ids.push_back(RenderTargetCollection::CreateColorTexture(internal_format, pixel_format, pixel_data_type, _fbo_width, _fbo_height));   
        _color_texture_formats.push_back(internal_format);
        _color_texture_formats.push_back(pixel_format);
        _color_texture_formats.push_back(pixel_data_type);
    }

    void BindDepthToSampler(GLint sampler_number)
    {
        OpenGL::ActivateTextureUnit(sampler_number);            
        glBindTexture(GL_TEXTURE_2D, _depth_texture_id);
    }

    void BindTargetToSampler(GLint target_number, GLint sampler_number)
    {
        OpenGL::ActivateTextureUnit(sampler_number);            
        glBindTexture(GL_TEXTURE_2D, _color_texture_ids[target_number]);
    }

    GLvoid RemoveTextureTarget(unsigned int target_number)
    {            
        glDeleteTextures(1, &_color_texture_formats[target_number]);      
        _color_texture_ids[target_number] = 0;
        _color_texture_formats[target_number*3] = GL_NONE;
        _color_texture_formats[target_number*3 + 1] = GL_NONE;
        _color_texture_formats[target_number*3 + 2] = GL_NONE;
        vector<GLuint> new_texture_ids;            
        for (unsigned int i = 0; i < _color_texture_ids.size(); ++i) 
        {
            GLuint texture = _color_texture_ids[i];
            if (texture != 0) new_texture_ids.push_back(texture);        
        }               
        _color_texture_ids = new_texture_ids;
        vector<GLenum> new_texture_formats;
        for (unsigned int i = 0; i < _color_texture_formats.size(); ++i) 
        {
            GLenum format = _color_texture_formats[i];
            if (format != GL_NONE) new_texture_formats.push_back(format);                      
        } 
        _color_texture_formats = new_texture_formats;
    }
            
    void SetDepthTextureDataFormat(GLenum depth_texture_data_format)
    {
        if (_depth_texture_id)
			glDeleteTextures(1, &_depth_texture_id);
        _depth_texture_data_format = depth_texture_data_format;
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);                
        _depth_texture_id = RenderTargetCollection::CreateDepthTexture(_depth_texture_data_format, _fbo_width, _fbo_height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_texture_id, 0);
    }
};

#endif
