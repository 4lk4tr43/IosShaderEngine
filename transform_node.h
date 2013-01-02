#ifndef transform_node_h__
#define transform_node_h__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
using namespace glm;

#include "node.h"

class TransformNode : public Node
{
public:
    vec3 position;
    quat rotation;
    
    void Move(vec3 &right_up_forward)
    {
        auto rotation_matrix = mat3_cast(rotation);
        auto movement_vector = column(rotation_matrix, 0) * right_up_forward.x;
        movement_vector = column(rotation_matrix, 1) * right_up_forward.y;
        movement_vector = column(rotation_matrix, 2) * right_up_forward.z;
        position += movement_vector;
    }
    
    void Rotate(vec3 &pitch_yaw_roll)
    {
        quat increase_rotation(pitch_yaw_roll);
        rotation = rotation * increase_rotation;
        position = increase_rotation * position;
    }
    
    mat4x4 ViewMatrix()
    {
        return transpose(inverse(WorldMatrix()));
    }

    mat4x4 WorldMatrix()
    {
        return translate(mat4_cast(rotation), position);
    }
};

#endif
