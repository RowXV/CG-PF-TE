/*
Proyecto Final Teoria
*/

//Cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//Iluminacion
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//Variables
//Variables animacion 
//Animacion compleja Orbe
float movZigZag = 0.0f;					//Movimiento en el eje z de Orbe
float movVert = 0.0f;					//Movimiento en el eje y de Orbe
float movOffset;
bool dir = true;						//Para cambiar la direccion en el eje y

//Animacion basica Mineral
float movAnimBas2;						//Movimiento de Mineral
float movAnimBas2Offset = true;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Camara
Camera camera;

//Texturas a utilizar en entorno opengl
Texture pisoTexture;	//Textura de piso

//Modelos a utilizar en entorno opengl

//Edificaciones
Model PSol;

//Objetos
Model Lifmunk;

//Skybox a utilizar en entorno opengl
//Dos tipos para el dia y la noche
Skybox skybox;

//Materiales a utilizar en entorno opengl
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

//Para el piso
void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 1.0f,		0.0f, -1.0f, 0.0f,	//0
		10.0f, 0.0f, -10.0f,	1.0f, 1.0f,	0.0f, -1.0f, 0.0f,	//1
		-10.0f, 0.0f, 10.0f,	0.0f, 0.0f,	0.0f, -1.0f, 0.0f,	//2
		10.0f, 0.0f, 10.0f,		1.0f, 0.0f,	0.0f, -1.0f, 0.0f	//3
	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	//********************************CARGA DE TEXTURAS*************************************
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	//********************************CARGA DE MODELOS*************************************
	//Objetos
	Lifmunk = Model();
	Lifmunk.LoadModel("Models/Lifmunk.obj");

	//Edificaciones
	PSol = Model();
	PSol.LoadModel("Models/PSol.obj");

	std::vector<std::string> skyboxFaces;

	skyboxFaces.push_back("Textures/Skybox/skybox_3.tga"); //right
	skyboxFaces.push_back("Textures/Skybox/skybox_1.tga"); //left
	skyboxFaces.push_back("Textures/Skybox/skybox_6.tga"); //down
	skyboxFaces.push_back("Textures/Skybox/skybox_5.tga"); //up
	skyboxFaces.push_back("Textures/Skybox/skybox_2.tga"); //front
	skyboxFaces.push_back("Textures/Skybox/skybox_4.tga"); //bh

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	//LUCES PUNTUALES
	//Contador de luces puntuales
	unsigned int pointLightCount = 0;

	//*********************************+***LUZ DE LA JOYA*************************************
	pointLights[0] = PointLight(0.3f, 0.3f, 1.0f,
		0.0f, 1.0f,
		244.0f, 15.0f, -114.0f, 
		0.0075f, 0.005f, 0.0025f);
	pointLightCount++;
	//*********************************+***LUZ DEL ORO*************************************
	pointLights[1] = PointLight(1.0f, 1.0f, 0.3f,
		0.5f, 0.001f,
		-417.0f, 3.0f, 232.0f,
		0.75f, 0.005f, 0.01f);
	pointLightCount++;
	//*********************************+***LUZ DEL ORBE*************************************
	pointLights[2] = PointLight(0.3f, 1.0f, 0.3f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0075f, 0.01f, 0.005f);
	pointLightCount++;


	//LUCES SPOTLIGHT
	//Contador de luces spotlight
	unsigned int spotLightCount = 0;

	//Primera luz Spotlight
	//*********************************+***LUZ DEL CARRO*************************************
	spotLights[0] = SpotLight(0.3f, 0.3f, 1.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0003f, 0.0002f,
		15.0f);
	spotLightCount++;
	//*********************************+***LUZ DEL VOCHO*************************************
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.3f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0003f, 0.0002f,
		15.0f);
	spotLightCount++;
	//*********************************+***LUZ DEL VOCHO*************************************
	spotLights[2] = SpotLight(1.0f, 0.3f, 0.3f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0003f, 0.0002f,
		15.0f);
	spotLightCount++;

	//Continuar para más luces

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	//Loop mientras no se cierra la ventana
	
	

	//Variable animacion Orbe
	movOffset = 0.05f;

	movAnimBas2 = 0.0f;
	movAnimBas2Offset = 0.5f;


	lastTime = glfwGetTime(); //Para empezar lo más cercano posible a 0

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Animacion Mineral
		movAnimBas2 += movAnimBas2Offset * deltaTime;

		//Animacion Compleja Orbe
		movZigZag += 5.0f * deltaTime;
		if (dir == true)
		{
			movVert += movOffset * deltaTime;
			
			if (movVert > 8.0f)
			{
				dir = false;
			}
		}
		else if (dir == false)
		{
			movVert -= movOffset * deltaTime;
			if (movVert < 0.0f)
			{
				dir = true;
			}
		}
		
		//Recibir eventos del usuario
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		//**************************************************************************PISO**************************************************************************
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 1.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//**************************************************************************EDIFICACIONES**************************************************************************

		//PIRAMIDE DEL SOL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-247.0f, 0.0f, 245.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		PSol.RenderModel();
		glDisable(GL_BLEND);

		//**************************************************************************OBJETOS**************************************************************************

		//Mineral (1)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-248.0f, 29.3f, 300.0f));
		model = glm::rotate(model, movAnimBas2 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lifmunk.RenderModel();

		

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
