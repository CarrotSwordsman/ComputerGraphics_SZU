#include "Angel.h"
#include "TriMesh.h"

#include <vector>
#include <fstream>
#include <string>

// #pragma comment(lib, "glew32.lib")

using namespace std;

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

const int TRANSFORM_SCALE = 0;
const int TRANSFORM_ROTATE = 1;
const int TRANSFORM_TRANSLATE = 2;

const double DELTA_DELTA = 0.3;		// Delta的变化率
const double DEFAULT_DELTA = 0.5;	// 默认的Delta值

double scaleDelta = DEFAULT_DELTA;
double rotateDelta = DEFAULT_DELTA;
double translateDelta = DEFAULT_DELTA;

glm::vec3 scaleTheta(1.0, 1.0, 1.0);		// 缩放控制变量
glm::vec3 rotateTheta(0.0, 0.0, 0.0);    // 旋转控制变量
glm::vec3 translateTheta(0.0, 0.0, 0.0);	// 平移控制变量

int currentTransform = TRANSFORM_ROTATE;	// 设置当前变换
int mainWindow;

int window;

struct openGLObject
{
	// 顶点数组对象
	GLuint vao;
	// 顶点缓存对象
	GLuint vbo;

	// 着色器程序
	GLuint program;
	// 着色器文件
	std::string vshader;
	std::string fshader;
	// 着色器变量
	GLuint pLocation;
	GLuint cLocation;
	GLuint matrixLocation;
	GLuint darkLocation;
};

openGLObject cube_object;

TriMesh* cube = new TriMesh();

// 三角面片中的顶点序列
//typedef struct vIndex {
//	unsigned int a, b, c;
//	vIndex(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
//} vec3i;

std::vector<glm::vec3> vertices;
std::vector<vec3i> faces;

int nVertices = 0;
int nFaces = 0;
int nEdges = 0;

std::vector<glm::vec3> points;   // 传入着色器的绘制点
std::vector<glm::vec3> colors;   // 传入着色器的颜色

const int NUM_VERTICES = 8;

const glm::vec3 vertex_colors[NUM_VERTICES] = {
	glm::vec3(1.0, 1.0, 1.0),  // White
	glm::vec3(1.0, 1.0, 0.0),  // Yellow
	glm::vec3(0.0, 1.0, 0.0),  // Green
	glm::vec3(0.0, 1.0, 1.0),  // Cyan
	glm::vec3(1.0, 0.0, 1.0),  // Magenta
	glm::vec3(1.0, 0.0, 0.0),  // Red
	glm::vec3(0.0, 0.0, 0.0),  // Black
	glm::vec3(0.0, 0.0, 1.0)   // Blue
};

void read_off(const std::string filename)
{
	// fin打开文件读取文件信息
	if (filename.empty()) {
		return;
	}
	std::ifstream fin;
	fin.open(filename);
	// @TODO: Task1:修改此函数读取OFF文件中三维模型的信息
	if (!fin)
	{
		printf("文件有误\n");
		return;
	}
	else
	{
		printf("文件打开成功\n");
		vertices.clear();
		faces.clear();

		// 读取OFF字符串
		string str;
		fin >> str;
		// 读取文件中顶点数、面片数、边数
		fin >> nVertices >> nFaces >> nEdges;

		// 根据顶点数，循环读取每个顶点坐标，将其保存到vertices
		for (int i = 0; i < nVertices; i++)
		{
			glm::vec3 tmp_node;
			fin >> tmp_node.x >> tmp_node.y >> tmp_node.z;
			vertices.push_back(tmp_node);
			//colors.push_back(tmp_node);
		}
		
		// 根据面片数，循环读取每个面片信息，并用构建的vec3i结构体保存到faces
		for (int i = 0; i < nFaces; i++)
		{
			int num, a, b, c;
			fin >> num >> a >> b >> c;
			faces.push_back(vec3i(a, b, c));
		}
	}
	fin.close();
}

void storeFacesPoints()
{
	points.clear();
	colors.clear();
	// @TODO: Task1:修改此函数在points和colors容器中存储每个三角面片的各个点和颜色信息
	// 在points容器中，依次添加每个面片的顶点，并在colors容器中，添加该点的颜色信息
	// 比如一个正方形由两个三角形构成，那么vertices会由4个顶点的数据构成，faces会记录两个三角形的顶点下标，
	// 而points就是记录这2个三角形的顶点，总共6个顶点的数据。
	// colors容器则是和points的顶点一一对应，保存这个顶点的颜色，这里我们可以使用顶点坐标或者自己设定的颜色赋值。
	for (int  i = 0; i < nFaces; i++)
	{
		points.push_back(vertices[faces[i].x]);
		colors.push_back(vertex_colors[faces[i].x]);
		points.push_back(vertices[faces[i].y]);
		colors.push_back(vertex_colors[faces[i].y]);
		points.push_back(vertices[faces[i].z]);
		colors.push_back(vertex_colors[faces[i].z]);
	}
}

void init()
{

	// 读取off模型文件
	cube->readOff("./Models/cow.off");
	storeFacesPoints();

	// 创建顶点数组对象
	GLuint vao[1];
    glGenVertexArrays(1, vao);  	// 分配1个顶点数组对象
	glBindVertexArray(vao[0]);  	// 绑定顶点数组对象

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3) + colors.size() * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);

	// @TODO: Task1:修改完成后再打开下面注释，否则程序会报错
	// 分别读取数据
	//glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), &points[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), &colors[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->getPoints().size() * sizeof(glm::vec3), &mesh->getPoints()[0]);
	glBufferSubData(GL_ARRAY_BUFFER, mesh->getPoints().size() * sizeof(glm::vec3), mesh->getColors().size() * sizeof(glm::vec3), &mesh->getColors()[0]);

	// 读取着色器并使用
	std::string vshader, fshader;
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	GLuint program = InitShader(vshader.c_str(), fshader.c_str());
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// 从顶点着色器中初始化顶点的颜色
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(glm::vec3)));

	// 黑色背景
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display()
{
	// 清理窗口
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(cube_object.program);

	glBindVertexArray(cube_object.vao);

	// 初始化变换矩阵
	glm::mat4 m(1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	// @TODO: Task4-在此处修改函数，计算最终的变换矩阵
	// 调用函数传入三种变化的变化量，累加得到变化矩阵
	// 注意三种变化累加的顺序


	// 从指定位置matrixLocation中传入变换矩阵m
	glUniformMatrix4fv(cube_object.matrixLocation, 1, GL_FALSE, glm::value_ptr(m));

	// 绘制立方体中的各个三角形
	glDrawArrays(GL_TRIANGLES, 0, cube->getPoints().size());
}

// 通过Delta值更新Theta
void updateTheta(int axis, int sign) {
	switch (currentTransform) {
		// 根据变换类型，增加或减少某种变换的变化量
	case TRANSFORM_SCALE:
		scaleTheta[axis] += sign * scaleDelta;
		break;
	case TRANSFORM_ROTATE:
		rotateTheta[axis] += sign * rotateDelta;
		break;
	case TRANSFORM_TRANSLATE:
		translateTheta[axis] += sign * translateDelta;
		break;
	}
}

// 复原Theta和Delta
void resetTheta()
{
	scaleTheta = glm::vec3(1.0, 1.0, 1.0);
	rotateTheta = glm::vec3(0.0, 0.0, 0.0);
	translateTheta = glm::vec3(0.0, 0.0, 0.0);
	scaleDelta = DEFAULT_DELTA;
	rotateDelta = DEFAULT_DELTA;
	translateDelta = DEFAULT_DELTA;
}

// 更新变化Delta值
void updateDelta(int sign)
{
	switch (currentTransform) {
		// 根据变化类型增加或减少每一次变化的单位变化量
	case TRANSFORM_SCALE:
		scaleDelta += sign * DELTA_DELTA;
		break;
	case TRANSFORM_ROTATE:
		rotateDelta += sign * DELTA_DELTA;
		break;
	case TRANSFORM_TRANSLATE:
		translateDelta += sign * DELTA_DELTA;
		break;
	}
}

// 窗口键盘回调函数。
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
		// 退出。
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
		break;
		// 1：缩放。
	case GLFW_KEY_1:
		if (action == GLFW_PRESS) currentTransform = TRANSFORM_SCALE;
		break;
		// 2: 旋转。
	case GLFW_KEY_2:
		if (action == GLFW_PRESS) currentTransform = TRANSFORM_ROTATE;
		break;
		// 3: 移动。
	case GLFW_KEY_3:
		if (action == GLFW_PRESS) currentTransform = TRANSFORM_TRANSLATE;
		break;
		// 4: 绘制线。
	case GLFW_KEY_4:
		if (action == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
		// 5: 绘制面。
	case GLFW_KEY_5:
		if (action == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);;
		break;
		// Q: 增加 x。
	case GLFW_KEY_Q:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) updateTheta(X_AXIS, 1);
		break;
		// A: 减少 x。
	case GLFW_KEY_A:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) updateTheta(X_AXIS, -1);
		break;
		// W: 增加 y。
	case GLFW_KEY_W:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) updateTheta(Y_AXIS, 1);
		break;
		// S: 减少 x。
	case GLFW_KEY_S:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) updateTheta(Y_AXIS, -1);
		break;
		// E: 增加 z。
	case GLFW_KEY_E:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) updateTheta(Z_AXIS, 1);
		break;
		// D: 减少 z。
	case GLFW_KEY_D:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) updateTheta(Z_AXIS, -1);
		break;
		// R: 增加变化量。
	case GLFW_KEY_R:
		if (action == GLFW_PRESS) updateDelta(1);
		break;
		// F: 减少变化量。
	case GLFW_KEY_F:
		if (action == GLFW_PRESS) updateDelta(-1);
		break;
		// T: 所有值重置。
	case GLFW_KEY_T:
		if (action == GLFW_PRESS) resetTheta();
		break;
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char** argv)
{
	// 初始化GLFW库，必须是应用程序调用的第一个GLFW函数
	glfwInit();

	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(600, 600, "3D OFF Model", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 调用任何OpenGL的函数之前初始化GLAD
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	init();
	while (!glfwWindowShouldClose(window))
	{
		display();

		// 交换颜色缓冲 以及 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}

// 每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理。
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

