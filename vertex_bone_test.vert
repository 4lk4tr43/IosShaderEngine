attribute vec3 position;
attribute vec3 normal;
attribute vec4 bone_ids;
attribute vec4 bone_weights;

uniform mat4 model_view_projection_matrix;
uniform mat4 normal_matrix;
uniform mat4 bones[20];

varying vec3 normal_var;

void main()
{
	mat4 bone_mat = bones[int(bone_ids[0])] * bone_weights[0];
 	bone_mat += bones[int(bone_ids[1])] * bone_weights[1];
 	bone_mat += bones[int(bone_ids[2])] * bone_weights[2];
 	bone_mat += bones[int(bone_ids[3])] * bone_weights[3];

	gl_Position = (model_view_projection_matrix * bone_mat) * vec4(position, 1.0);

	normal_var = mat3(normal_matrix) * normal;
}