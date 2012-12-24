#ifndef camera_h__
#define camera_h__

#include "glm/ext.hpp"
#include "glm/glm.hpp"
using namespace glm;

#include "opengl.h"

#ifndef CAMERA_REFERENCE_RIGHT
#define CAMERA_REFERENCE_RIGHT Vector3<GLfloat>(1.0f, 0.0f, 0.0f)
#endif

#ifndef CAMERA_REFERENCE_UP
#define CAMERA_REFERENCE_UP Vector3<GLfloat>(0.0f, 1.0f, 0.0f)
#endif

#ifndef CAMERA_REFERENCE_BACKWARDS
#define CAMERA_REFERENCE_BACKWARDS Vector3<GLfloat>(0.0f, 0.0f, -1.0f)
#endif

class Camera
{
	mat3x3 _rotation;
	quat _rotationQuaterion;
	vec3 _translation;		

	bool _quaterionNeedsUpdate, _matrixNeedsUpdate;

	void UpdateMatrix()
	{
		if (!_matrixNeedsUpdate) return;

		UpdateQuaterion();
		_rotation = Matrix3x3<GLfloat>::FromQuaterion(_rotationQuaterion);
		_matrixNeedsUpdate = false;
	}
	void UpdateQuaterion()
	{
		if (!_quaterionNeedsUpdate) return;

		_rotationQuaterion = _rotation.ToQuaterion();
		_quaterionNeedsUpdate = false;
	}

public:
	Camera() 
	{
		_rotation = Matrix3x3<GLfloat>::Identity();
		_rotationQuaterion = Vector4<GLfloat>::IdentityQuaterion();
		_translation(0.0f, 0.0f, 0.0f);

		_quaterionNeedsUpdate = false;
		_matrixNeedsUpdate = false;
	}

	Matrix3x3<GLfloat> RotationMatrix()
	{
		UpdateMatrix();
		return _rotation;
	}
	void RotationMatrix(Matrix3x3<GLfloat> val)
	{
		_rotation = val;

		_matrixNeedsUpdate = false;
		_quaterionNeedsUpdate = true;
	}
	Vector4<GLfloat> RotationQuaterion()
	{
		UpdateQuaterion();
		return _rotationQuaterion;
	}
	void RotationQuaterion(Vector4<GLfloat> val)
	{
		_rotationQuaterion = val;

		_matrixNeedsUpdate = true;
		_quaterionNeedsUpdate = false;
	}
	void Rotation(float pitch, float yaw, float roll)
	{
		_rotationQuaterion = Vector4<GLfloat>::QuaterionFromEulerAngles(pitch, yaw, roll);

		_matrixNeedsUpdate = true;
		_quaterionNeedsUpdate = false;
	}
	void Rotation(Vector3<GLfloat> val) 
	{
		Rotation(val.x, val.y, val.z);
	}
	Vector3<GLfloat> Position() { return _translation; }
	void Position(Vector3<GLfloat> val) { _translation = val; }

	Vector3<GLfloat> Target() { return Position() - _rotation.GetColumn(2); }
	void Target(Vector3<GLfloat> val) 
	{
		Vector3<GLfloat> forward = val - Position();
		Vector3<GLfloat> left = CAMERA_REFERENCE_UP.Cross(forward).Normalize();
		Vector3<GLfloat> up = forward.Cross(left).Normalize();

		_rotation.SetColumn(0, left);
		_rotation.SetColumn(1, up);
		_rotation.SetColumn(2, forward);

		_matrixNeedsUpdate = false;
		_quaterionNeedsUpdate = true;
	}

	void MoveForward(float val)
	{
		UpdateMatrix();
		_translation += _rotation.Forward() * val;
	}
	void MoveRight(float val)
	{
		UpdateMatrix();
		_translation -= _rotation.Right() * val;
	}
	void MoveUp(float val)
	{
		UpdateMatrix();
		_translation -= _rotation.Up() * val;
	}
	void IncreasePitch(float rad)
	{
		UpdateQuaterion();
		_rotationQuaterion = _rotationQuaterion * Vector4<GLfloat>::QuaterionFromAxisAngle(CAMERA_REFERENCE_RIGHT, rad);

		_matrixNeedsUpdate = true;
	}
	void IncreaseYaw(float rad)
	{
		UpdateQuaterion();
		_rotationQuaterion = _rotationQuaterion * Vector4<GLfloat>::QuaterionFromAxisAngle(CAMERA_REFERENCE_UP, rad);

		_matrixNeedsUpdate = true;
	}
	void IncreaseRoll(float rad)
	{
		UpdateQuaterion();
		_rotationQuaterion = _rotationQuaterion * Vector4<GLfloat>::QuaterionFromAxisAngle(CAMERA_REFERENCE_BACKWARDS, rad);

		_matrixNeedsUpdate = true;
	}

	Matrix4x4<GLfloat> ViewMatrix()
	{
		UpdateMatrix();

		return Matrix4x4<GLfloat>(_rotation, _translation);
	}
};	

#endif
