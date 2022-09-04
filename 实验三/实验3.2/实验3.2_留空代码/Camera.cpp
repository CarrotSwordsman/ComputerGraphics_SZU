#include "Camera.h"

Camera::Camera() { updateCamera(); };
Camera::~Camera() {};

// 获取观察矩阵
glm::mat4 Camera::getViewMatrix()
{
	return this->lookAt(eye, at, up);
}

// 获取投影矩阵
glm::mat4 Camera::getProjectionMatrix(bool isOrtho)
{
	// 使用正交
	if (isOrtho) {
		return this->ortho(-scale, scale, -scale, scale, this->zNear, this->zFar);
	}
	// 使用透视
	else {
		return this->perspective(fov, aspect, this->zNear, this->zFar);
	}
}


glm::mat4 Camera::lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	// @TODO: Task1:请按照实验课内容补全相机观察矩阵的计算
	glm::vec3 e(eye.x, eye.y, eye.z);
	glm::vec3 a(at.x, at.y, at.z);
	glm::vec3 VPN = e - a;
	glm::vec3 VUP(up.x, up.y, up.z);
	glm::vec3 n = glm::normalize(VPN);
	glm::vec3 u = glm::normalize(glm::cross(VUP, n));
	glm::vec3 v = glm::normalize(glm::cross(n, u));
	glm::mat4 viewMatrix(u.x, u.y, u.z, 0.0,
		v.x, v.y, v.z, 0.0,
		n.x, n.y, n.z, 0.0,
		0.0, 0.0, 0.0, 1.0);
	glm::mat4 T(1.0, 0.0, 0.0, -eye.x,
		0.0, 1.0, 0.0, -eye.y,
		0.0, 0.0, 1.0, -eye.z,
		0.0, 0.0, 0.0, 1.0);
	// 单位矩阵
	// glm::mat4 T = glm::mat4(1.0f);
	// p[0][3] = -(eye.x);
	// p[1][3] = -(eye.y);
	// p[2][3] = -(eye.z);
	viewMatrix = T * viewMatrix;
	return viewMatrix;
}

glm::mat4 Camera::ortho(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{	
	// @TODO: Task1:请按照实验课内容补全
	glm::mat4 c = glm::mat4(0.0f);
	c[0][0] = 2.0 / (right - left);
	c[1][1] = 2.0 / (top - bottom);
	c[2][2] = -2.0 / (zFar - zNear);
	c[3][3] = 1.0;
	c[0][3] = -(right + left) / (right - left);
	c[1][3] = -(top + bottom) / (top - bottom);
	c[2][3] = -(zFar + zNear) / (zFar - zNear);
	return c;
}

glm::mat4 Camera::perspective(const GLfloat fovy, const GLfloat aspect,
	const GLfloat zNear, const GLfloat zFar)
{
	// @TODO: Task1:请按照实验课内容补全
	GLfloat top = tan(fov * M_PI / 180 / 2) * zNear;
	GLfloat right = top * aspect;
	glm::mat4 c = glm::mat4(0.0f);
	c[0][0] = zNear / right;
	c[1][1] = zNear / top;
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
	c[3][2] = -1.0;
	return c;
}

glm::mat4 Camera::frustum(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// 任意视锥体矩阵
	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = 2.0 * zNear / (right - left);
	c[0][2] = (right + left) / (right - left);
	c[1][1] = 2.0 * zNear / (top - bottom);
	c[1][2] = (top + bottom) / (top - bottom);
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;
	return c;
}


void Camera::updateCamera()
{
	// 使用相对于at的角度控制相机的时候，注意在upAngle大于90的时候，相机坐标系的u向量会变成相反的方向，
	// 要将up的y轴改为负方向才不会发生这种问题

	// 也可以考虑直接控制相机自身的俯仰角，
	// 保存up，eye-at 这些向量，并修改这些向量方向来控制
	// 看到这里的有缘人可以试一试
	up = glm::vec4(0.0, 1.0, 0.0, 0.0);
	if (upAngle > 90){
		up.y = -1;
	}
	else if (upAngle < -90){
		up.y = -1;
	}
	float eyex = radius * cos(upAngle * M_PI / 180.0) * sin(rotateAngle * M_PI / 180.0);
	float eyey = radius * sin(upAngle * M_PI / 180.0);
	float eyez = radius * cos(upAngle * M_PI / 180.0) * cos(rotateAngle * M_PI / 180.0);

	eye = glm::vec4(eyex, eyey, eyez, 1.0);
	at = glm::vec4(0.0, 0.0, 0.0, 1.0);
	// up = vec4(0.0, 1.0, 0.0, 0.0);

}

void Camera::keyboard(int key, int action, int mode)
{
	// 键盘事件处理

	if(key == GLFW_KEY_U && action == GLFW_PRESS && mode == 0x0000) 
	{
		rotateAngle += 5.0;
	}
	else if(key == GLFW_KEY_U && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) 
	{
		rotateAngle -= 5.0;
	}
	else if(key == GLFW_KEY_I && action == GLFW_PRESS && mode == 0x0000) 
	{
		upAngle += 5.0;
		if (upAngle > 180)
			upAngle = 180;
	}
	else if(key == GLFW_KEY_I && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) 
	{
		upAngle -= 5.0;
		if (upAngle < -180)
			upAngle = -180;
	}
	else if(key == GLFW_KEY_O && action == GLFW_PRESS && mode == 0x0000) 
	{
		scale += 0.1;
	}
	else if(key == GLFW_KEY_O && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) 
	{
		scale -= 0.1;
	}
	else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS && mode == 0x0000) 
	{
		radius = 4.0;
		rotateAngle = 0.0;
		upAngle = 30.0;
		fov = 45.0;
		aspect = 1.0;
		scale = 1.5;
	}
}
