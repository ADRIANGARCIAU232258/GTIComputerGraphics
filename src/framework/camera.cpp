
#include "camera.h"

#include "main/includes.h"
#include <iostream>
#include <cmath> // Librería para funciones trigonométricas

Camera::Camera()
{
	view_matrix.SetIdentity();
	SetOrthographic(-1,1,1,-1,-1,1);
}

Vector3 Camera::GetLocalVector(const Vector3& v)
{
	Matrix44 iV = view_matrix;
	if (iV.Inverse() == false)
		std::cout << "Matrix Inverse error" << std::endl;
	Vector3 result = iV.RotateVector(v);
	return result;
}

Vector3 Camera::ProjectVector(Vector3 pos, bool& negZ)
{
	Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
	Vector4 result = viewprojection_matrix * pos4;
	negZ = result.z < 0;
	if (type == ORTHOGRAPHIC)
		return result.GetVector3();
	else
		return result.GetVector3() / result.w;
}

void Camera::Rotate(float angle, const Vector3& axis)
{
	Matrix44 R;
	R.SetRotation(angle, axis);
	Vector3 new_front = R * (center - eye);
	center = eye + new_front;
	UpdateViewMatrix();
}

void Camera::Move(Vector3 delta)
{
	Vector3 localDelta = GetLocalVector(delta);
	eye = eye - localDelta;
	center = center - localDelta;
	UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
	type = ORTHOGRAPHIC;

	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::SetPerspective(float fov, float aspect, float near_plane, float far_plane)
{
	type = PERSPECTIVE;

	this->fov = fov;
	this->aspect = aspect;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	this->eye = eye;
	this->center = center;
	this->up = up;

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	// Reseteamos la matriz a identidad
	view_matrix.SetIdentity();

	// A continuación creamos la matriz de vista usando vectores normalizados
	Vector3 zaxis = (eye - center).Normalize();    // Eje Z de la cámara
	Vector3 xaxis = (up.Cross(zaxis)).Normalize(); // Eje X
	Vector3 yaxis = zaxis.Cross(xaxis);            // Eje Y

	// Asignamos los valores calculados a la matriz de vista
	view_matrix.M[0][0] = xaxis.x; view_matrix.M[0][1] = yaxis.x; view_matrix.M[0][2] = zaxis.x; view_matrix.M[0][3] = 0;
	view_matrix.M[1][0] = xaxis.y; view_matrix.M[1][1] = yaxis.y; view_matrix.M[1][2] = zaxis.y; view_matrix.M[1][3] = 0;
	view_matrix.M[2][0] = xaxis.z; view_matrix.M[2][1] = yaxis.z; view_matrix.M[2][2] = zaxis.z; view_matrix.M[2][3] = 0;

	// Aplicamos la traslación local a la matriz de vista
	view_matrix.M[3][0] = -xaxis.Dot(eye);
	view_matrix.M[3][1] = -yaxis.Dot(eye);
	view_matrix.M[3][2] = -zaxis.Dot(eye);
	view_matrix.M[3][3] = 1;

	// Aplicamos la traslación local usando las coordenadas de la cámara
	view_matrix.TranslateLocal(-eye.x, -eye.y, -eye.z);

	// Actualizamos la matriz de vista proyección
	UpdateViewProjectionMatrix();
}

void Camera::UpdateProjectionMatrix()
{
	// Reseteamos la matriz a identidad
	projection_matrix.SetIdentity();

	if (type == PERSPECTIVE) {
		// Calculamos el valor de la tangente del ángulo FOV dividido en 2
		float tanHalfFOV = std::tan(fov / 2.0f);
		// Asignamos los valores a la matriz de proyección para perspectiva
		projection_matrix.M[0][0] = 1.0f / (aspect * tanHalfFOV);
		projection_matrix.M[1][1] = 1.0f / tanHalfFOV;
		projection_matrix.M[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
		projection_matrix.M[2][3] = -1.0f;
		projection_matrix.M[3][2] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
		projection_matrix.M[3][3] = 0.0f;
	}
	else if (type == ORTHOGRAPHIC) {
		// Asignamos los valores a la matriz de proyección para ortográfica
		projection_matrix.M[0][0] = 2.0f / (right - left);
		projection_matrix.M[1][1] = 2.0f / (top - bottom);
		projection_matrix.M[2][2] = -2.0f / (far_plane - near_plane);
		projection_matrix.M[3][0] = -(right + left) / (right - left);
		projection_matrix.M[3][1] = -(top + bottom) / (top - bottom);
		projection_matrix.M[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
		projection_matrix.M[3][3] = 1.0f;
	}

	// Actualizamos la matriz de vista proyección
	UpdateViewProjectionMatrix();
}


void Camera::UpdateViewProjectionMatrix()
{
	viewprojection_matrix = projection_matrix * view_matrix;
}

Matrix44 Camera::GetViewProjectionMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	return viewprojection_matrix;
}

// The following methods have been created for testing.
// Do not modify them.

void Camera::SetExampleViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m );
}

void Camera::SetExampleProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type == PERSPECTIVE)
		gluPerspective(fov, aspect, near_plane, far_plane);
	else
		glOrtho(left,right,bottom,top,near_plane,far_plane);

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m );
	glMatrixMode(GL_MODELVIEW);
}
