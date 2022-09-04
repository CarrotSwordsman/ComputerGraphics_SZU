#version 330 core

// 给光源数据一个结构体
struct Light{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	vec3 position;
};

// 给物体材质数据一个结构体
struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float shininess;
};

in vec3 position;
in vec3 normal;

// 相机坐标
uniform vec3 eye_position;
// 光源
uniform Light light;
// 物体材质
uniform Material material;

uniform int isShadow;

in vec3 color;
out vec4 fColor;

void main()
{
	if (isShadow == 0) {
		fColor = vec4(0.0, 0.0, 0.0, 1.0);	//黑色
	}
	else if (isShadow == 1){
		fColor = vec4(0.5, 0.5, 0.5, 1.0);	//灰色
	}
	else if (isShadow == 2){
		fColor = vec4(color, 1.0);
	}
	else {

		// 将顶点坐标、光源坐标和法向量转换到相机坐标系
		vec3 norm = (vec4(normal, 0.0)).xyz;
		
		// @TODO: 计算四个归一化的向量 N,V,L,R(或半角向量H)
		vec3 N = normalize(norm);
		vec3 V = normalize(eye_position - position);
		vec3 L = normalize(light.position - position);
		vec3 R = reflect(-L, N);


		// 环境光分量I_a
		vec4 I_a = light.ambient * material.ambient;

		// @TODO: Task2 计算系数和漫反射分量I_d
		float diffuse_dot = max(dot(L, N), 0);
		vec4 I_d = diffuse_dot *  light.diffuse * material.diffuse;

		// @TODO: Task2 计算系数和镜面反射分量I_s
		float specular_dot_pow = pow(max(dot(R, V), 0), material.shininess);
		vec4 I_s = specular_dot_pow * light.specular * material.specular;


		// @TODO: Task2 计算高光系数beta和镜面反射分量I_s
		// 注意如果光源在背面则去除高光
		if( dot(L, N) < 0.0 ) {
		    I_s = vec4(0.0, 0.0, 0.0, 1.0);
		}
		
		// 合并三个分量的颜色，修正透明度
		fColor = I_a + I_d + I_s;
		fColor.a = 1.0;
		
	}
}

