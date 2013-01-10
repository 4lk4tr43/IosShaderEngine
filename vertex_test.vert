attribute vec3 position;
attribute vec3 normal;

uniform mat4 model_view_projection_matrix;
uniform mat4 normal_matrix;

varying vec3 normal_var;

void main()
{
	normal_var = mat3(normal_matrix) * normal;
	gl_Position = model_view_projection_matrix * vec4(position, 1);
}