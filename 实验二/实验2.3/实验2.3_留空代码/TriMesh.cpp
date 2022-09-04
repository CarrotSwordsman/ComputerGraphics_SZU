#include "TriMesh.h"


// 一些基础颜色
const glm::vec3 basic_colors[8] = {
	glm::vec3(1.0, 1.0, 1.0),	// White
	glm::vec3(1.0, 1.0, 0.0),	// Yellow
	glm::vec3(0.0, 1.0, 0.0),	// Green
	glm::vec3(0.0, 1.0, 1.0),	// Cyan
	glm::vec3(1.0, 0.0, 1.0),	// Magenta
	glm::vec3(1.0, 0.0, 0.0),	// Red
	glm::vec3(0.0, 0.0, 0.0),	// Black
	glm::vec3(0.0, 0.0, 1.0),	// Blue
};

// 立方体的各个点
const glm::vec3 cube_vertices[8] = {
	glm::vec3(-0.5, -0.5, -0.5),
	glm::vec3(0.5, -0.5, -0.5),
	glm::vec3(-0.5,  0.5, -0.5),
	glm::vec3(0.5,  0.5, -0.5),
	glm::vec3(-0.5, -0.5,  0.5),
	glm::vec3(0.5, -0.5,  0.5),
	glm::vec3(-0.5,  0.5,  0.5),
	glm::vec3(0.5,  0.5,  0.5)
};


TriMesh::TriMesh()
{
}

TriMesh::~TriMesh()
{
}

std::vector<glm::vec3> TriMesh::getVertexPositions()
{
	return vertex_positions;
}

std::vector<glm::vec3> TriMesh::getVertexColors()
{
	return vertex_colors;
}

std::vector<vec3i> TriMesh::getFaces()
{
	return faces;
}


std::vector<glm::vec3> TriMesh::getPoints()
{
	return points;
}

std::vector<glm::vec3> TriMesh::getColors()
{
	return colors;
}

void TriMesh::cleanData() {
	vertex_positions.clear();
	vertex_colors.clear();	
	
	faces.clear();

	points.clear();
	colors.clear();
}

void TriMesh::storeFacesPoints() {

	// @TODO: Task-2修改此函数在points和colors容器中存储每个三角面片的各个点和颜色信息
	// 根据每个三角面片的顶点下标存储要传入GPU的数据
    for (int i = 0; i < 12; i++)
    {
        points.push_back(vertex_positions[faces[i].x]);
        colors.push_back(vertex_colors[faces[i].x]);
        points.push_back(vertex_positions[faces[i].y]);
        colors.push_back(vertex_colors[faces[i].y]);
        points.push_back(vertex_positions[faces[i].z]);
        colors.push_back(vertex_colors[faces[i].z]);
    }

}

// 立方体生成12个三角形的顶点索引
void TriMesh::generateCube() {
	// 创建顶点前要先把那些vector清空
	cleanData();

	// @TODO: Task1-修改此函数，存储立方体的各个面信息
    // vertex_positions和vertex_colors先保存每个顶点的数据
    for (int i = 0; i < 8; i++)
    {
        vertex_positions.push_back(cube_vertices[i]);
        vertex_colors.push_back(basic_colors[i]);
    }
    // faces再记录每个面片上顶点的下标
    faces.push_back(vec3i(1, 0, 2));
    faces.push_back(vec3i(1, 3, 2));
    faces.push_back(vec3i(2, 3, 6));
    faces.push_back(vec3i(3, 7, 6));
    faces.push_back(vec3i(5, 6, 7));
    faces.push_back(vec3i(5, 6, 4));
    faces.push_back(vec3i(1, 4, 0));
    faces.push_back(vec3i(1, 4, 5));
    faces.push_back(vec3i(3, 1, 5));
    faces.push_back(vec3i(3, 7, 5));
    faces.push_back(vec3i(2, 4, 0));
    faces.push_back(vec3i(2, 4, 6));

	storeFacesPoints();
}



void TriMesh::readOff(const std::string& filename)
{
    // fin打开文件读取文件信息
    if (filename.empty())
    {
        return;
    }
    std::ifstream fin;
    fin.open(filename);
    if (!fin)
    {
        printf("File on error\n");
        return;
    }
    else
    {
        printf("File open success\n");
		cleanData();
		int nVertices, nFaces, nEdges;

        // 读取OFF字符串
        std::string str;
        fin >> str;
        // 读取文件中顶点数、面片数、边数
        fin >> nVertices >> nFaces >> nEdges;
        // 根据顶点数，循环读取每个顶点坐标
        for (int i = 0; i < nVertices; i++)
        {
			glm::vec3 tmp_node;
            fin >> tmp_node.x >> tmp_node.y >> tmp_node.z;
            vertex_positions.push_back(tmp_node);
			vertex_colors.push_back(tmp_node);
        }
        // 根据面片数，循环读取每个面片信息，并用构建的vec3i结构体保存
        for (int i = 0; i < nFaces; i++)
        {
            int num, a, b, c;
            // num记录此面片由几个顶点构成，a、b、c为构成该面片顶点序号
            fin >> num >> a >> b >> c;
            faces.push_back(vec3i(a, b, c));
        }
    }
    fin.close();
    storeFacesPoints();
};