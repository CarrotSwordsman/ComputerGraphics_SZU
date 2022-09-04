/*
 *        Computer Graphics Course - Shenzhen University
 *    Mid-term Assignment - Tetris implementation sample code
 * ============================================================
 *
 * - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
 * - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
 *
 * - 已实现功能如下：
 * - 1) 绘制棋盘格和‘L’型方块
 * - 2) 键盘左/右/下键控制方块的移动，上键旋转方块
 * - 3) 绘制‘J’、‘Z’等形状的方块
 * - 4) 随机生成方块并赋上不同的颜色
 * - 5) 方块的自动向下移动
 * - 6) 方块之间、方块与边界之间的碰撞检测
 * - 7) 棋盘格中每一行填充满之后自动消除
 * - 8) 输出提示信息
 * - 9）在游戏开始前选择游戏难度
 * -10）通过P键控制游戏暂停与继续
 * -11）根据游戏难度积分
 * 
 * 
 *——————————以下功能已经全部实现————————————————
 * - 未实现功能如下：
 * - 1) 绘制‘J’、‘Z’等形状的方块
 * - 2) 随机生成方块并赋上不同的颜色
 * - 3) 方块的自动向下移动
 * - 4) 方块之间、方块与边界之间的碰撞检测
 * - 5) 棋盘格中每一行填充满之后自动消除
 * - 6) 其他
 *————————————————————————————————————————————
 */

#include "Angel.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <time.h>
#include <windows.h>

double starttime;	// 控制方块向下移动时间
double nowtime;		// 记录当前时间
int rotation;		// 控制当前窗口中的方块旋转
glm::vec2 tile[4];		// 表示当前窗口中的方块
bool gameover = true;	// 游戏结束控制变量
int xsize = 400;		// 窗口大小（尽量不要变动窗口大小！）
int ysize = 720;
double DownNum = 0;		// 记录下落了多少方块
double DownTime = 1;	// 控制下落时间间隔
bool pause = false;		// 控制游戏暂停与继续
int Difficulty;			// 游戏难度
int Points = 0;			// 游戏积分


// 单个网格大小
int tile_width = 33;

// 网格布大小
const int board_width = 10;
const int board_height = 20;

// 网格三角面片的顶点数量
const int points_num = board_height * board_width * 6;

// 我们用画直线的方法绘制网格
// 包含竖线 board_width+1 条
// 包含横线 board_height+1 条
// 一条线2个顶点坐标
// 网格线的数量
const int board_line_num =  (board_width + 1) + (board_height + 1);


// 一个二维数组表示所有可能出现的方块和方向
int type;	//新图形的随机颜色
glm::vec2 allRotationsshape[7][4][4] =
{
	//1."O"
	{ {glm::vec2(0, 0), glm::vec2(-1,0),glm::vec2(0, -1), glm::vec2(-1,-1)},
	{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},
	{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},
	{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)} },
	//2."I"
	{ {glm::vec2(-2, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(0,0)},
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(0, -2)},
	{glm::vec2(-2, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(0,0)},
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(0, -2)} },
	//3."S"
	{ {glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(-1, -1), glm::vec2(1,0)},
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1,0), glm::vec2(1, -1)},
	{glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(-1, -1), glm::vec2(1,0)},
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1,0), glm::vec2(1, -1)} },
	//4."Z"
	{ {glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(0, -1), glm::vec2(1,-1)},
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1,0), glm::vec2(1, 1)},
	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(0, -1), glm::vec2(1,-1)},
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1,0), glm::vec2(1, 1)} },
	//5."L"
	{ {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(-1,-1)},
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(1, -1)},
	{glm::vec2(1, 1), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,  0)},
	{glm::vec2(-1,1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1)} },
	//6."J"
	{ {glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(1,-1)},
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(1, 1)},
	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(-1,  1)},
	{glm::vec2(-1,-1), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1)} },
	//7."T"
	{ {glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(0,-1)},
	{glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0,1), glm::vec2(1, 0)},
	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(0,  1)},
	{glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1)} }
};

// 绘制窗口的颜色变量
int col;
int COL_NUM = 6;
glm::vec4 orange = glm::vec4(1.0, 0.5, 0.0, 1.0);
glm::vec4 white  = glm::vec4(1.0, 1.0, 1.0, 1.0);
glm::vec4 black  = glm::vec4(0.0, 0.0, 0.0, 1.0);
glm::vec4 red  = glm::vec4(1.0, 0.0, 0.0, 1.0);
glm::vec4 green  = glm::vec4(0.0, 1.0, 0.0, 1.0);
glm::vec4 blue  = glm::vec4(0.0, 0.0, 1.0, 1.0);
glm::vec4 pink  = glm::vec4(1.0, 0.75, 0.8, 1.0);
glm::vec4 yellow  = glm::vec4(1.0, 1.0, 0.0, 1.0);
glm::vec4 BasicColor[] = { orange, red, green, blue, pink, yellow };

// 当前方块的位置（以棋盘格的左下角为原点的坐标系）
glm::vec2 tilepos = glm::vec2(5, 19);

// 布尔数组表示棋盘格的某位置是否被方块填充，即board[x][y] = true表示(x,y)处格子被填充。
// （以棋盘格的左下角为原点的坐标系）
bool board[board_width][board_height];

// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
glm::vec4 board_colours[points_num];

GLuint locxsize;
GLuint locysize;

GLuint vao[3];
GLuint vbo[6];

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO
void changecellcolour(glm::vec2 pos, glm::vec4 colour)
{
	// 每个格子是个正方形，包含两个三角形，总共6个定点，并在特定的位置赋上适当的颜色
	for (int i = 0; i < 6; i++)
		board_colours[(int)( 6 * ( board_width*pos.y + pos.x) + i)] = colour;

	glm::vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour};

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

	// 计算偏移量，在适当的位置赋上颜色
	int offset = 6 * sizeof(glm::vec4) * (int)( board_width * pos.y + pos.x);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 当前方块移动或者旋转时，更新VBO
void updatetile()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

	// 每个方块包含四个格子
	for (int i = 0; i < 4; i++)
	{
		// 计算格子的坐标值
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;

		glm::vec4 p1 = glm::vec4(tile_width + (x * tile_width), tile_width + (y * tile_width), .4, 1);
		glm::vec4 p2 = glm::vec4(tile_width + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);
		glm::vec4 p3 = glm::vec4(tile_width*2 + (x * tile_width), tile_width + (y * tile_width), .4, 1);
		glm::vec4 p4 = glm::vec4(tile_width*2 + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);

		// 每个格子包含两个三角形，所以有6个顶点坐标
		glm::vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(glm::vec4), 6*sizeof(glm::vec4), newpoints);
	}
	#ifdef __APPLE__
		glBindVertexArrayAPPLE(0);
	#else
		glBindVertexArray(0);
	#endif
}
void fuzhi(glm::vec2 a[4][4], glm::vec2 b[4][4])
{

}
// 设置当前方块为下一个即将出现的方块。在游戏继续的时候调用来创建一个初始的方块，
// 在游戏结束的时候判断，没有足够的空间来生成新的方块。
bool checkvalid(glm::vec2 cellpos);
bool movetile(glm::vec2 direction);
void newtile()
{
	// 判断中间最上面两行是否被填满，若填满则游戏继续
	for (int j = 18; j < board_height; j++)
	{
		for (int i = 4; i < 7; i++)
		{
			if (board[i][j])
			{ 
				gameover = true;
				printf("GAME OVER!\n");
				printf("R:Restart the game\n");
				printf("Q:Quit the game\n");
				return;
			}
		}
	}
	// 将新方块放于棋盘格的最上行中间位置并设置随机的旋转方向
	time_t t;
	srand((unsigned)time(&t));
	rotation = rand() % 4;
	type = rand() % 7;
	tilepos = glm::vec2(5, 19);
	if (!(checkvalid((allRotationsshape[type][rotation][0]) + tilepos)
		&& checkvalid((allRotationsshape[type][rotation][1]) + tilepos)
		&& checkvalid((allRotationsshape[type][rotation][2]) + tilepos)
		&& checkvalid((allRotationsshape[type][rotation][3]) + tilepos)))
	{
		tilepos = glm::vec2(5, 18);
	}
	//rotation = 0;

	//每次随机生成新图形
	for (int i = 0; i < 4; i++)
	{
		tile[i] = allRotationsshape[type][rotation][i];
	}

	updatetile();

	// 给新方块赋上颜色
	col = rand() % COL_NUM;
	glm::vec4 newcolours[24];
	for (int i = 0; i < 24; i++)
		newcolours[i] = BasicColor[col];

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

// 游戏和OpenGL初始化
void init()
{
	printf("GAME START!\n");
	printf("Here are the operating tips:\n");
	printf("Please select the difficulty level from 1 to 5 first before starting the game\n\n");
	printf("P:Pause or start the game\n");
	printf("R:Restart the game\n");
	printf("Q:Quit the game\n");
	printf("↑:Rotate the block\n");
	printf("←:Move the block to the right\n");
	printf("→:Move the block to the right\n");
	printf("↓:Accelerate the falling of the block\n\n");
	// 初始化棋盘格，这里用画直线的方法绘制网格
	// 包含竖线 board_width+1 条
	// 包含横线 board_height+1 条
	// 一条线2个顶点坐标，并且每个顶点一个颜色值
	
	glm::vec4 gridpoints[board_line_num * 2];
	glm::vec4 gridcolours[board_line_num * 2];

	// 绘制网格线
	// 纵向线
	for (int i = 0; i < (board_width+1); i++)
	{
		gridpoints[2*i] = glm::vec4((tile_width + (tile_width * i)), tile_width, 0, 1);
		gridpoints[2*i + 1] = glm::vec4((tile_width + (tile_width * i)), (board_height+1) * tile_width, 0, 1);
	}

	// 水平线
	for (int i = 0; i < (board_height+1); i++)
	{
		gridpoints[ 2*(board_width+1) + 2*i ] = glm::vec4(tile_width, (tile_width + (tile_width * i)), 0, 1);
		gridpoints[ 2*(board_width+1) + 2*i + 1 ] = glm::vec4((board_width+1) * tile_width, (tile_width + (tile_width * i)), 0, 1);
	}

	// 将所有线赋成白色
	for (int i = 0; i < (board_line_num * 2); i++)
		gridcolours[i] = white;

	// 初始化棋盘格，并将没有被填充的格子设置成黑色
	glm::vec4 boardpoints[points_num];
	for (int i = 0; i < points_num; i++)
		board_colours[i] = black;

	// 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子
	for (int i = 0; i < board_height; i++)
		for (int j = 0; j < board_width; j++)
		{
			glm::vec4 p1 = glm::vec4(tile_width + (j * tile_width), tile_width + (i * tile_width), .5, 1);
			glm::vec4 p2 = glm::vec4(tile_width + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
			glm::vec4 p3 = glm::vec4(tile_width*2 + (j * tile_width), tile_width + (i * tile_width), .5, 1);
			glm::vec4 p4 = glm::vec4(tile_width*2 + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
			boardpoints[ 6 * ( board_width * i + j ) + 0 ] = p1;
			boardpoints[ 6 * ( board_width * i + j ) + 1 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 2 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 3 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 4 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 5 ] = p4;
		}

	// 将棋盘格所有位置的填充与否都设置为false（没有被填充）
	for (int i = 0; i < board_width; i++)
		for (int j = 0; j < board_height; j++)
			board[i][j] = false;

	// 载入着色器
	std::string vshader, fshader;
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	GLuint program = InitShader(vshader.c_str(), fshader.c_str());
	glUseProgram(program);

	locxsize = glGetUniformLocation(program, "xsize");
	locysize = glGetUniformLocation(program, "ysize");

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vColor = glGetAttribLocation(program, "vColor");

	
	glGenVertexArrays(3, &vao[0]);
	glBindVertexArray(vao[0]);		// 棋盘格顶点
	
	glGenBuffers(2, vbo);

	// 棋盘格顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridcolours, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(vao[1]);		// 棋盘格每个格子

	glGenBuffers(2, &vbo[2]);

	// 棋盘格每个格子顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格每个格子顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), board_colours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(vao[2]);		// 当前方块

	glGenBuffers(2, &vbo[4]);

	// 当前方块顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 当前方块顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(0);

	glClearColor(0, 0, 0, 0);

	//// 游戏初始化
	//if (!gameover) {
	//	newtile();
	//	starttime = glfwGetTime();
	//}
}

// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内
bool checkvalid(glm::vec2 cellpos)
{
	if((cellpos.x >=0) && (cellpos.x < board_width) 
		&& (cellpos.y >= 0) && (cellpos.y < board_height)	//判断是否在窗口内
		&&(board[(int)cellpos.x][(int)cellpos.y] == false)	//判断是否与其他方块重叠
		)
		return true;
	else
		return false;
}

// 在棋盘上有足够空间的情况下旋转当前方块
void rotate()
{
	// 计算得到下一个旋转方向
	int nextrotation = (rotation + 1) % 4;

	// 检查当前旋转之后的位置的有效性
	if (checkvalid((allRotationsshape[type][nextrotation][0]) + tilepos)
		&& checkvalid((allRotationsshape[type][nextrotation][1]) + tilepos)
		&& checkvalid((allRotationsshape[type][nextrotation][2]) + tilepos)
		&& checkvalid((allRotationsshape[type][nextrotation][3]) + tilepos))
	{
		// 更新旋转，将当前方块设置为旋转之后的方块
		rotation = nextrotation;
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsshape[type][rotation][i];

		updatetile();
	}
}

// 检查棋盘格在row行有没有被填充满
void checkfullrow(int row)
{
	int flag = 1;	//判断是否填满
	for (int i = 0; i < board_width; i++)
	{
		//若该行未被填满则跳出
		if (board[i][row] == false)
			flag = 0;
	}
	//如果填满，则将最后一行消去，并将上面所有格子下移
	if (flag)
	{
		Points += 10 * Difficulty;
		printf("Points:%d\n", Points);
		//将该行置为false,并且颜色改为黑色
		for (int i = 0; i < board_width; i++)
		{
			changecellcolour(glm::vec2(i, row), black);
			board[i][row] = false;
		}
		//将该行以上所有格子下移
		for (int j = row + 1; j < board_height; j++)
		{
			for (int i = 0; i < board_width; i++)
			{
				if (board[i][j])
				{
					board[i][j - 1] = true;
					board[i][j] = false;
					changecellcolour(glm::vec2(i, j - 1), //先对下移的格子填色
						board_colours[6 * (i + j * board_width)]);
					changecellcolour(glm::vec2(i, j), black);
				}
			}
		}
	}
}

// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO
void settile()
{
	// 每个格子
	for (int i = 0; i < 4; i++)
	{
		// 获取格子在棋盘格上的坐标
		int x = (tile[i] + tilepos).x;
		int y = (tile[i] + tilepos).y;
		// 将格子对应在棋盘格上的位置设置为填充
		board[x][y] = true;
		// 并将相应位置的颜色修改
		changecellcolour(glm::vec2(x, y), BasicColor[col]);
	}
	// 检查方块的每个格子是否都可以消除
	// 实现同时消去多行的操作
	for (int i = 0; i < 4; i++)
	{
		int y = (tile[i] + tilepos).y;
		checkfullrow(y);
	}
}

// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向下和向右移动。如果移动成功，返回值为true，反之为false
bool movetile(glm::vec2 direction)
{
	// 计算移动之后的方块的位置坐标
	glm::vec2 newtilepos[4];
	for (int i = 0; i < 4; i++)
		newtilepos[i] = tile[i] + tilepos + direction;

	// 检查移动之后的有效性
	if (checkvalid(newtilepos[0])
		&& checkvalid(newtilepos[1])
		&& checkvalid(newtilepos[2])
		&& checkvalid(newtilepos[3])) 
	{
		//检查是否与周围方块有重叠
		for (int i = 0; i < 4; i++)
		{
			int xx = newtilepos[i].x;
			int yy = newtilepos[i].y;
			if (board[xx][yy] == 1)
			{
				return false;
			}
		}
		// 有效：移动该方块
		tilepos.x = tilepos.x + direction.x;
		tilepos.y = tilepos.y + direction.y;
		updatetile();
		return true;	
	}
	return false;
}

// 重新启动游戏
void restart()
{
	// 将所有格子填为黑色并置为false
	for (int j = 0; j < board_height; j++)
	{
		for (int i = 0; i < board_width; i++)
		{
			board[i][j] = false;
			changecellcolour(glm::vec2(i, j), black);
		}
	}
	for (int i = 0; i < points_num; i++)
		board_colours[i] = black;
	// 将所有数据初始化
	starttime = glfwGetTime();
	DownNum = 1;
	DownTime = 1;
	Difficulty = 1;
	Points = 0;
	gameover = false;
	newtile();
}

// 游戏渲染部分
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize);
	glUniform1i(locysize, ysize);

	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, points_num); // 绘制棋盘格 (width * height * 2 个三角形)
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLES, 0, 24);	 // 绘制当前方块 (8 个三角形)
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_LINES, 0, board_line_num * 2 );		 // 绘制棋盘格的线

}

// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

// 键盘响应事件中的特殊按键响应
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(!gameover)
	{
		switch(key)
		{	
			// 控制方块的移动方向，更改形态
			case GLFW_KEY_UP:	// 向上按键旋转方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT)
				{
					rotate();
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_DOWN: // 向下按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					if (!movetile(glm::vec2(0, -1)))
					{
						settile();
						newtile();
						break;
					}
					else
					{
						break;
				}
				}
			case GLFW_KEY_LEFT:  // 向左按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					movetile(glm::vec2(-1, 0));
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_RIGHT: // 向右按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					movetile(glm::vec2(1, 0));
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_P:	// 控制游戏暂停与继续
				if (action == GLFW_PRESS) 
				{
					if (!pause)
						pause = true;
					else
						pause = false;
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_ESCAPE:
				if (action == GLFW_PRESS) {
					exit(EXIT_SUCCESS);
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_Q:
				if (action == GLFW_PRESS) {
					exit(EXIT_SUCCESS);
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_R:
				if (action == GLFW_PRESS) {
					restart();
					break;
				}
				else
				{
					break;
				}
		}
	}
	// 若游戏已经结束
	else
	{
		switch (key)
		{
			case GLFW_KEY_ESCAPE:
				if (action == GLFW_PRESS) {
					exit(EXIT_SUCCESS);
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_Q:
				if (action == GLFW_PRESS) {
					exit(EXIT_SUCCESS);
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_R:
				if (action == GLFW_PRESS) {
					restart();
					break;
				}
				else
				{
					break;
				}
			// 选择游戏难度1
			case GLFW_KEY_1:
				if (action == GLFW_PRESS) {
					DownTime = 1;
					Difficulty = 1;
					gameover = false;
					break;
				}
				else
				{
					break;
				}
			// 选择游戏难度2
			case GLFW_KEY_2:
				if (action == GLFW_PRESS) {
					DownTime = 0.8;
					Difficulty = 2;
					gameover = false;
					break;
				}
				else
				{
					break;
				}
			// 选择游戏难度3
			case GLFW_KEY_3:
				if (action == GLFW_PRESS) {
					DownTime = 0.6;
					Difficulty = 3;
					gameover = false;
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_4:
				if (action == GLFW_PRESS) {
					DownTime = 0.4;
					Difficulty = 4;
					gameover = false;
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_5:
				if (action == GLFW_PRESS) {
					DownTime = 0.2;
					Difficulty = 5;
					gameover = false;
					break;
				}
				else
				{
					break;
				}
		}	
	} 
}

void down()
{
	nowtime = glfwGetTime();	// 当前时间
	if (nowtime - starttime > DownNum * DownTime)
	{
		// 如果游戏时间大于限定的下落时间，则方块持续下落
		DownNum++;
		if (!gameover && !pause)
		{
			if (!movetile(glm::vec2(0, -1)))
			{
				settile();
				newtile();
			}
		}
	}
}

int main(int argc, char **argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// 创建窗口。
	GLFWwindow* window = glfwCreateWindow(500, 900, "2019085071-shiyichuan-Midterm", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	
	init();
	int flag = 0;
	while (!glfwWindowShouldClose(window))
    { 
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
		// 难度选择后游戏正式继续
		if (!gameover && !flag) {
			newtile();
			starttime = glfwGetTime();
			printf("Difficulty:%d\n\n", Difficulty);
			flag = 1;
		}
		down();
    }
    glfwTerminate();
    return 0;
}
