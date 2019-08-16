// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"

#define DEGTORAD(x) ((pi/180)*x)

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& _axis) 
{
	const vec3& axis = glm::normalize(_axis);
	float rads = DEGTORAD(degrees);
	return 
		(cos(rads) * 	mat3(1.0f)) + 
		((1-cos(rads)) * glm::outerProduct(axis, axis)) + 
		(sin(rads) * 	mat3(0, axis.z, -axis.y, -axis.z, 0, axis.x, axis.y, -axis.x, 0));
}

mat3 Transform::rotate(const float degrees, vec3&& axis) 
{
	return rotate(degrees, axis);
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
	eye = eye * rotate(-degrees, up);
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
	vec3 right = glm::cross(glm::normalize(up), glm::normalize(eye));
	eye = eye * rotate(degrees, right);
	up = up * rotate(degrees, right);
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
	vec3 w = glm::normalize(eye - center); // norm(a)
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

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
	float d = tan(DEGTORAD(fovy) / 2.0f);
	float doa = 1.0f / (aspect * d);
	float A = -(zFar + zNear) / (zFar - zNear);
	float B = -((2.0f * zFar * zNear) / (zFar - zNear));
    return { 
		doa, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f/d, 0.0f, 0.0f,
		0.0f, 0.0f, A, -1.0f,
		0.0f, 0.0f, B, 0.0f
	};
    // return { 
	// 	doa, 0.0f, 0.0f, 0.0f,
	// 	0.0f, d, 0.0f, 0.0f,
	// 	0.0f, 0.0f, A, B,
	// 	0.0f, 0.0f, -1.0f, 0.0f
	// };
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    return {
		sx, 0.0f, 0.0f, 0.0f,
		0.0f, sy, 0.0f, 0.0f,
		0.0f, 0.0f, sz, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 
		tx, ty, tz, 1.0f
	};
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
