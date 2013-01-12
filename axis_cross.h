#ifndef axis_cross_h__
#define axis_cross_h__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace glm;

#include "opengl.h"
#include "shader.h"
#include "vertex_array_object.h"

#define AXIS_CROSS_VERTEX_SHADER (GLchar*)"attribute vec3 position;attribute vec4 color;uniform mat4 model_view_projection;varying vec4 color_var;void main(){gl_Position=model_view_projection*vec4(position,1.0);color_var=color;}"
#define AXIS_CROSS_VERTEX_SHADER_ATTRIB_NAMES (GLchar*)"position,color"
#define AXIS_CROSS_SHADER_UNIFORM_NAMES (GLchar*)"model_view_projection"
#define AXIS_CROSS_FRAGMENT_SHADER (GLchar*) "varying vec4 color_var;void main(){gl_FragColor=color_var;}"

static unsigned int _axis_cross_reference_count = 0;
static Shader *_axis_cross_shader = nullptr;
static VertexArrayObject *_axis_cross_vao_array_object = nullptr;

class AxisCross
{
	void InitVao()
	{
		if (_axis_cross_reference_count)
			return;
		GLfloat axis_cross_vertices[27] =
		{
			.1f, .1f, .1f,
			1.0f, .0f, .0f,
			.0f, .0f, .0f,

			.1f, .1f, .1f,
			.0f, 1.0f, .0f,
			.0f, .0f, .0f,

			.1f, .1f, .1f,
			.0f, .0f, 1.0f,
			.0f, .0f, .0f
		};
		GLfloat axis_cross_colors[36] =
		{
			1.0f, .0f, .0f, 1.0f,
			1.0f, .0f, .0f, 1.0f,
			1.0f, .0f, .0f, 1.0f,

			.0f, 1.0f, .0f, 1.0f,
			.0f, 1.0f, .0f, 1.0f,
			.0f, 1.0f, .0f, 1.0f,

			.0f, .0f, 1.0f, 1.0f,
			.0f, .0f, 1.0f, 1.0f,
			.0f, .0f, 1.0f, 1.0f
		};
		vector<GLvoid*> vertices;
		vertices.push_back((GLvoid*)axis_cross_vertices);
		vertices.push_back((GLvoid*)axis_cross_colors);
		_axis_cross_vao_array_object = new VertexArrayObject();
		_axis_cross_vao_array_object->AddVertices(GL_STATIC_DRAW, GL_TRIANGLES, VertexDescription::PositionColor(), vertices, 9);	
	}

	void InitShader() 
	{
		if (_axis_cross_reference_count)
			return;
		string error_log;
		_axis_cross_shader = new Shader(AXIS_CROSS_VERTEX_SHADER, AXIS_CROSS_FRAGMENT_SHADER, AXIS_CROSS_VERTEX_SHADER_ATTRIB_NAMES, AXIS_CROSS_SHADER_UNIFORM_NAMES, &error_log);

	}

public:
	AxisCross()
	{		
		InitVao();
		InitShader();
		++_axis_cross_reference_count;
	}

	~AxisCross()
	{
		if (!--_axis_cross_reference_count)
		{
			delete _axis_cross_shader;
			_axis_cross_shader = nullptr;
			delete _axis_cross_vao_array_object;
			_axis_cross_vao_array_object = nullptr;
		}
	}

	void Draw(mat4 &model_view_projection_matrix, GLfloat scale = 1.0f)
	{
		_axis_cross_shader->Activate();
		glUniformMatrix4fv(_axis_cross_shader->UniformID(0), 1, GL_FALSE, value_ptr(model_view_projection_matrix * glm::scale(mat4(1.0f),vec3(scale))));
		_axis_cross_vao_array_object->DrawRaw();
	}
};

#endif