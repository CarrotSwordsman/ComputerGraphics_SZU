#include "MeshPainter.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

MeshPainter::MeshPainter() {};
MeshPainter::~MeshPainter() {};

std::vector<std::string> MeshPainter::getMeshNames() { return mesh_names; };
std::vector<TriMesh*> MeshPainter::getMeshes() { return meshes; };
std::vector<openGLObject> MeshPainter::getOpenGLObj() { return opengl_objects; };

void MeshPainter::bindObjectAndData(TriMesh* mesh, openGLObject& object, const std::string& texture_image, const std::string& vshader, const std::string& fshader) {
    // ��ʼ�����ֶ���

    std::vector<glm::vec3> points = mesh->getPoints();
    std::vector<glm::vec3> normals = mesh->getNormals();
    std::vector<glm::vec3> colors = mesh->getColors();
    std::vector<glm::vec2> textures = mesh->getTextures();

    // ���������������
#ifdef __APPLE__	// for MacOS
    glGenVertexArraysAPPLE(1, &object.vao);		// ����1�������������
    glBindVertexArrayAPPLE(object.vao);		// �󶨶����������
#else				// for Windows
    glGenVertexArrays(1, &object.vao);  	// ����1�������������
    glBindVertexArray(object.vao);  	// �󶨶����������
#endif

    // ��������ʼ�����㻺�����
    glGenBuffers(1, &object.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    glBufferData(GL_ARRAY_BUFFER,
        points.size() * sizeof(glm::vec3) +
        normals.size() * sizeof(glm::vec3) +
        colors.size() * sizeof(glm::vec3) +
        textures.size() * sizeof(glm::vec2),
        NULL, GL_STATIC_DRAW);

    // �󶨶�������
    glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), points.data());
    // ����ɫ����
    glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), colors.data());
    // �󶨷���������
    glBufferSubData(GL_ARRAY_BUFFER, (points.size() + colors.size()) * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), normals.data());
    // ����������
    glBufferSubData(GL_ARRAY_BUFFER, (points.size() + normals.size() + colors.size()) * sizeof(glm::vec3), textures.size() * sizeof(glm::vec2), textures.data());


    object.vshader = vshader;
    object.fshader = fshader;
    object.program = InitShader(object.vshader.c_str(), object.fshader.c_str());

    // �����㴫����ɫ��
    object.pLocation = glGetAttribLocation(object.program, "vPosition");
    glEnableVertexAttribArray(object.pLocation);
    glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // ����ɫ������ɫ��
    object.cLocation = glGetAttribLocation(object.program, "vColor");
    glEnableVertexAttribArray(object.cLocation);
    glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(glm::vec3)));

    // ��������������ɫ��
    object.nLocation = glGetAttribLocation(object.program, "vNormal");
    glEnableVertexAttribArray(object.nLocation);
    glVertexAttribPointer(object.nLocation, 3,
        GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET((points.size() + colors.size()) * sizeof(glm::vec3)));

    object.tLocation = glGetAttribLocation(object.program, "vTexture");
    glEnableVertexAttribArray(object.tLocation);
    glVertexAttribPointer(object.tLocation, 2,
        GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET((points.size() + colors.size() + normals.size()) * sizeof(glm::vec3)));


    // ��þ���λ��
    object.modelLocation = glGetUniformLocation(object.program, "model");
    object.viewLocation = glGetUniformLocation(object.program, "view");
    object.projectionLocation = glGetUniformLocation(object.program, "projection");

    object.shadowLocation = glGetUniformLocation(object.program, "isShadow");

    // ��ȡ����ͼƬ��
    object.texture_image = texture_image;
    // ��������Ļ������
    glGenTextures(1, &object.texture);
    // ����stb_image��������
    load_texture_STBImage(object.texture_image, object.texture);

    // Clean up
    glUseProgram(0);
#ifdef __APPLE__
    glBindVertexArrayAPPLE(0);
#else
    glBindVertexArray(0);
#endif

};


void MeshPainter::bindLightAndMaterial(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera) {
    // ���ݲ��ʡ���Դ�����ݸ���ɫ��

    // ���������λ��
    glUniform3fv(glGetUniformLocation(object.program, "eye_position"), 1, &camera->eye[0]);

    // ��������Ĳ���
    glm::vec4 meshAmbient = mesh->getAmbient();
    glm::vec4 meshDiffuse = mesh->getDiffuse();
    glm::vec4 meshSpecular = mesh->getSpecular();
    float meshShininess = mesh->getShininess();

    glUniform4fv(glGetUniformLocation(object.program, "material.ambient"), 1, &meshAmbient[0]);
    glUniform4fv(glGetUniformLocation(object.program, "material.diffuse"), 1, &meshDiffuse[0]);
    glUniform4fv(glGetUniformLocation(object.program, "material.specular"), 1, &meshSpecular[0]);
    glUniform1f(glGetUniformLocation(object.program, "material.shininess"), meshShininess);


    // ���ݹ�Դ��Ϣ
    glm::vec4 lightAmbient = light->getAmbient();
    glm::vec4 lightDiffuse = light->getDiffuse();
    glm::vec4 lightSpecular = light->getSpecular();
    glm::vec3 lightPosition = light->getTranslation();
    glUniform4fv(glGetUniformLocation(object.program, "light.ambient"), 1, &lightAmbient[0]);
    glUniform4fv(glGetUniformLocation(object.program, "light.diffuse"), 1, &lightDiffuse[0]);
    glUniform4fv(glGetUniformLocation(object.program, "light.specular"), 1, &lightSpecular[0]);
    glUniform3fv(glGetUniformLocation(object.program, "light.position"), 1, &lightPosition[0]);

    glUniform1f(glGetUniformLocation(object.program, "light.constant"), light->getConstant());
    glUniform1f(glGetUniformLocation(object.program, "light.linear"), light->getLinear());
    glUniform1f(glGetUniformLocation(object.program, "light.quadratic"), light->getQuadratic());

}


void MeshPainter::addMesh(TriMesh* mesh, const std::string& name, const std::string& texture_image, const std::string& vshader, const std::string& fshader) {
    mesh_names.push_back(name);
    meshes.push_back(mesh);

    openGLObject object;
    // ��openGL���󣬲����ݶ������Ե�����
    bindObjectAndData(mesh, object, texture_image, vshader, fshader);

    opengl_objects.push_back(object);
};



void MeshPainter::drawMesh(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera) {

    // ����������
    camera->updateCamera();
    camera->viewMatrix = camera->getViewMatrix();
    camera->projMatrix = camera->getProjectionMatrix(true);

#ifdef __APPLE__	// for MacOS
    glBindVertexArrayAPPLE(object.vao);
#else
    glBindVertexArray(object.vao);
#endif
    glUseProgram(object.program);

    // ����ı任����
    glm::mat4 modelMatrix = mesh->getModelMatrix();
    // ���ݾ���
    glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
    glUniformMatrix4fv(object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
    // ����ɫ�� isShadow ����Ϊ2����ʾ�����������
    glUniform1i(object.shadowLocation, 2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object.texture);// �������룬����ֻʹ��ͬһ��������л���
    // ������������ �����ɵ�������shader
    glUniform1i(glGetUniformLocation(object.program, "texture"), 0);
    // �����ʺ͹�Դ���ݴ��ݸ���ɫ��
    bindLightAndMaterial(mesh, object, light, camera);
    // ����
    glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

    // @TODO: ���ݹ�Դλ�ã�������ӰͶӰ����
    glm::vec3 light_position = light->getTranslation();
    float lx = light_position[0];
    float ly = light_position[1];
    float lz = light_position[2];
    glm::mat4 shadowProjMatrix(
        -ly, 0.0, 0.0, 0.0,
        lx, 0.0, lz, 1.0,
        0.0, 0.0, -ly, 0.0,
        0.0, 0.0, 0.0, -ly);
    // ������Ӱ��ģ�ͱ任����
    modelMatrix = shadowProjMatrix * modelMatrix;
    // ���� isShadow ������3��ʾ��ɫ��
    glUniform1i(object.shadowLocation, 3);
    // ���� unifrom �ؼ��ֵľ������ݡ�
    glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    // ����
    glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

#ifdef __APPLE__	// for MacOS
    glBindVertexArrayAPPLE(0);
#else
    glBindVertexArray(0);
#endif
    glUseProgram(0);

};


void MeshPainter::drawMeshes(Light* light, Camera* camera) {
    for (int i = 0; i < meshes.size(); i++)
    {
        drawMesh(meshes[i], opengl_objects[i], light, camera);
    }
};

void MeshPainter::cleanMeshes() {
    // �����ݶ�����ͷ�
    mesh_names.clear();

    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->cleanData();

        delete meshes[i];
        meshes[i] = NULL;

#ifdef __APPLE__
        glDeleteVertexArraysAPPLE(1, &opengl_objects[i].vao);
#else
        glDeleteVertexArrays(1, &opengl_objects[i].vao);
#endif
        glDeleteBuffers(1, &opengl_objects[i].vbo);
        glDeleteProgram(opengl_objects[i].program);
    }

    meshes.clear();
    opengl_objects.clear();
};


void MeshPainter::load_texture_STBImage(const std::string& file_name, GLuint& texture) {
    // ��ȡ����ͼƬ�������䴫�ݸ���ɫ��

    int width, height, channels = 0;
    unsigned char* pixels = NULL;
    // ��ȡͼƬ��ʱ���ȷ�תһ��ͼƬ����������õĻ���ʾ�����Ƿ�������ͼƬ
    stbi_set_flip_vertically_on_load(true);
    // ��ȡͼƬ����
    pixels = stbi_load(file_name.c_str(), &width, &height, &channels, 0);

    // �����ж����ʽ
    if (width * channels % 4 != 0)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLenum format = GL_RGB;
    // ����ͨ����ʽ
    switch (channels)
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        format = GL_RGB;
        break;
    }

    // ���������
    glBindTexture(GL_TEXTURE_2D, texture);

    // ָ������ķŴ���С�˲���ʹ�����Է�ʽ������ͼƬ�Ŵ��ʱ���ֵ��ʽ
    // ��ͼƬ��rgb�����ϴ���opengl
    glTexImage2D(
        GL_TEXTURE_2D,    // ָ��Ŀ���������ֵ������GL_TEXTURE_2D
        0,                // ִ��ϸ�ڼ���0���������ͼ�񼶱�n��ʾ��N����ͼϸ������
        format,           // �������ݵ���ɫ��ʽ(GPU�Դ�)
        width,            // ��ȡ����ڵ��Կ���֧�ֲ�������������Ⱥ͸߶ȱ�����2^n
        height,           // �߶ȡ����ڵ��Կ���֧�ֲ�������������Ⱥ͸߶ȱ�����2^n
        0,                // ָ���߿�Ŀ�ȡ�����Ϊ0
        format,           // �������ݵ���ɫ��ʽ(CPU�ڴ�)
        GL_UNSIGNED_BYTE, // ָ���������ݵ���������
        pixels            // ָ���ڴ���ָ��ͼ�����ݵ�ָ��
    );

    // ���ɶ༶��Զ����������1/3���Դ棬��С�ֱ���ʱ��ø��õ�Ч��
    // glGenerateMipmap(GL_TEXTURE_2D);

    // ָ����ֵ����
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // �ָ���ʼ�����ʽ
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    // �ͷ�ͼ���ڴ�
    stbi_image_free(pixels);
};