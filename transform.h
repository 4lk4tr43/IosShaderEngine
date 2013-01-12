#ifndef transform_h__
#define transform_h__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
using namespace glm;

class Transform
{
public:
    vec3 position;
    quat rotation;

	void LookAt(vec3 &position, vec3 &target, vec3 &up)
	{
		this->position = position;
		rotation = inverse(quat_cast(mat3(lookAt(position, target, up))));
	}

    void Move(vec3 &right_up_forward)
    {
        auto rotation_matrix = mat3_cast(rotation);
        vec3 movement_vector = column(rotation_matrix, 0) * right_up_forward.x;
        movement_vector += column(rotation_matrix, 1) * right_up_forward.y;
        movement_vector += column(rotation_matrix, 2) * right_up_forward.z;
        position += movement_vector;
    }

	Transform operator*(Transform &parent)
	{
		Transform result;
		result.position = parent.position + parent.rotation * position;
		result.rotation = parent.rotation * rotation;
		return result;
	}

    void Rotate(vec3 &pitch_yaw_roll)
    {
        rotation = rotation * quat(pitch_yaw_roll);
    }
    
    mat4x4 ViewMatrix()
    {
        return inverse(WorldMatrix());
    }

    mat4x4 WorldMatrix()
    {
        return translate(mat4(1.0f), position) * mat4_cast(rotation);
    }
};

#endif
