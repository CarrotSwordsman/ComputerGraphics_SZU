#include "Angel.h"
#include <string>

// #pragma comment(lib, "glew32.lib")

const int NUM_POINTS = 3;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void init()
{
	// 定义三角形的三个点
	glm::vec2 vertices[3] = {
		glm::vec2(-0.75, -0.75), glm::vec2(0.0, 0.75), glm::vec2(0.75, -0.75)
	};

	// 创建顶点数组对象
	GLuint vao;
	glGenVertexArrays(1, &vao);		// 分配1个顶点数组对象
    glBindVertexArray(vao);			// 绑定顶点数组对象

	// 创建顶点缓存对象
	GLuint buffer;
	// 分配1个顶点缓存对象
	glGenBuffers(1, &buffer);
	// 绑定顶点缓存对象
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// 分配数据所需的存储空间，将数据拷贝到OpenGL服务端内存
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 读取着色器并使用
	std::string vshader, fshader;
    vshader = "shaders/vshader.glsl";
    fshader = "shaders/fshader.glsl";
	GLuint program = InitShader(vshader.c_str(), fshader.c_str());
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint location = glGetAttribLocation(program, "vPosition");
	// 启用顶点属性数组
	glEnableVertexAttribArray(location);
	// 关联到顶点属性数组 (index, size, type, normalized, stride, *pointer)
	glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 白色背景
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display(void)
{
	// 清理窗口
	glClear(GL_COLOR_BUFFER_BIT);
	// 绘制所有点
	glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS);
	glFlush();
}

int main(int argc, char **argv)
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
	GLFWwindow* window = glfwCreateWindow(512, 512, "Red Triangle", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 调用任何OpenGL的函数之前初始化GLAD
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	init();
	// 指定当前窗口进行重绘时要调用的函数
	while (!glfwWindowShouldClose(window))
	{	
		display();

		// 交换颜色缓冲 以及 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Supported GLSL version is: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

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
