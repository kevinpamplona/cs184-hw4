// Transform.cpp: implementation of the Transform class.

#include "Transform.h"

mat3 Transform::rotate(const float degrees, const vec3& axis) {

	const float rads = glm::radians(degrees);
	
	const float x = axis[0];
	const float y = axis[1];
	const float z = axis[2];
	
	mat3 ident_mat(1.0);
	mat3 matrix_one = mat3(x*x, x*y, x*z, x*y, y*y, y*z, x*z, y*z, z*z);
	mat3 matrix_two = mat3(0, -z, y, z, 0, -x, -y, x, 0);
	
	mat3 rotate_matrix = cos(rads)*ident_mat + (1-cos(rads))*matrix_one + sin(rads)*matrix_two;
	
	return rotate_matrix;
}

void Transform::left(float degrees, vec3& eye, vec3& up) {
	eye = eye*rotate(degrees, up);
}

void Transform::up(float degrees, vec3& eye, vec3& up) {
	vec3 ortho_axis = glm::cross(eye, up);
	ortho_axis = glm::normalize(ortho_axis);
	
	eye = eye*rotate(degrees, ortho_axis);
	up = up*rotate(degrees, ortho_axis);
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {

	vec3 a = eye-center;
	vec3 b = up;
	
	vec3 w = glm::normalize(a);
	vec3 bXw = glm::cross(b,w);
	vec3 u = glm::normalize(bXw);
	vec3 v = glm::cross(w,u);
	
	mat4 rot_mat = mat4(u[0], u[1], u[2], 0, v[0], v[1], v[2], 0, w[0], w[1], w[2], 0, 0, 0, 0, 1);
	mat4 trans_mat = mat4(1, 0, 0, eye[0], 0, 1, 0, eye[1], 0, 0, 1, eye[2], 0, 0, 0, 1);
	
	float spot1 = -u[0]*eye[0]-u[1]*eye[1]-u[2]*eye[2];
	float spot2 = -v[0]*eye[0]-v[1]*eye[1]-v[2]*eye[2];
	float spot3 = -w[0]*eye[0]-w[1]*eye[1]-w[2]*eye[2];
	
	mat4 final_mat = mat4(u[0], u[1], u[2], spot1, v[0], v[1], v[2], spot2, w[0], w[1], w[2], spot3, 0, 0, 0, 1);
	
    return final_mat;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
	const float theta = glm::radians(fovy/2);
	const float d = cos(theta)/sin(theta);
	const float A = -((zFar+zNear)/(zFar-zNear));
	const float B = -((2*zFar*zNear)/(zFar-zNear));
	const float first = d/aspect;
	
	mat4 persp_mat = mat4(first, 0, 0, 0, 0, d, 0, 0, 0, 0, A, B, 0, 0, -1, 0);
	
	return persp_mat;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
	mat4 scale_mat = mat4(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
	return scale_mat;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) {
	mat4 trans_mat = mat4(1, 0, 0, tx, 0, 1, 0, ty, 0, 0, 1, tz, 0, 0, 0, 1);
	return trans_mat;
}

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) {
  vec3 x = glm::cross(up,zvec) ; 
  vec3 y = glm::cross(zvec,x) ; 
  vec3 ret = glm::normalize(y) ; 
  return ret ; 
}

mat4 Transform::homogen(mat3 matx) {
    mat4 temp = mat4(1.0);
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            temp[i][j]=matx[i][j];
        }
    }
    return temp;
}

Transform::Transform()
{

}

Transform::~Transform()
{

}
