// Transform.cpp: implementation of the Transform class.


#include "Transform.h"

// Please implement the following functions:

// Helper rotation function.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
	float rads = ((2*pi)/360)*degrees;
	return 
		(cos(rads) * 	mat3(1.0f)) + 
		((1-cos(rads)) * glm::outerProduct(axis, axis)) + 
		(sin(rads) * 	mat3(0, axis.z, -axis.y, -axis.z, 0, axis.x, axis.y, -axis.x, 0));
}

// Transforms the camera left around the "crystal ball" interface
void Transform::left(float degrees, vec3& eye, vec3& up) 
{
	eye = eye * rotate(-degrees, up);
}

// Transforms the camera up around the "crystal ball" interface
void Transform::up(float degrees, vec3& eye, vec3& up) 
{
	vec3 right = glm::cross(glm::normalize(up), glm::normalize(eye));
	eye = eye * rotate(degrees, right);
	up = up * rotate(degrees, right);
}

// Your implementation of the glm::lookAt matrix
mat4 Transform::lookAt(vec3 eye, vec3 up) 
{
	vec3 w = glm::normalize(eye); // norm(a)
	vec3 u = glm::normalize(glm::cross(up, w)); // norm(b x w)
	vec3 v = glm::cross(w, u); // w x u
	return mat4(
			u.x, v.x, w.x, 0.0f, 
			u.y, v.y, w.y, 0.0f, 
			u.z, v.z, w.z, 0.0f, 
			0.0f, 0.0f, 0.0f, 1.0f
		) * mat4(
			1.0f, 0.0f, 0.0f, 0.0f, 
			0.0f, 1.0f, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f, 0.0f, 
			-eye.x, -eye.y, -eye.z, 1.0f
		);
}

Transform::Transform()
{

}

Transform::~Transform()
{

}
