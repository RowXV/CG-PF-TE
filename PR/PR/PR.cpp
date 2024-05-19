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

//Variables animacion 

//Animacion Silla Bici
float movBici = 0.0f;					//Movimiento en el eje y de asiento
float movOffset;						//Velocidad

float movPuerta = 0.0f;					//Movimiento puerta
float movOffsetPuerta;					//Velocidad

float movCajon = 0.0f;					//Movimiento cajon
float movOffsetCajon;					//Velocidad
bool dirCajon = true;					//direccion


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Camara
Camera camera;

//Texturas a utilizar en entorno opengl
Texture pisoTexture;	//Textura de piso

//Modelos a utilizar en entorno opengl

//Edificaciones
Model Casa;

//Objetos
//Planta Baja
Model Taylor;
Model Cama;
Model Jarron;
Model Buro;
Model BuroPuerta;
Model BuroCajon;
Model Silla;
Model AsientoSilla;

//Planta Alta
Model SillDobl;
Model LamPared;
Model Mesa;
Model AsBike;
Model AsBikeSup;
Model SillInd;

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

	camera = Camera(glm::vec3(0.0f, 20.0f, 70.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);

	//********************************CARGA DE TEXTURAS*************************************
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTexture();

	//********************************CARGA DE MODELOS*************************************
	//Objetos
	//Planta Baja
	Taylor = Model();
	Taylor.LoadModel("Models/Taylor.obj");

	Cama = Model();
	Cama.LoadModel("Models/Cama.obj");

	Jarron = Model();
	Jarron.LoadModel("Models/Jarron.obj");

	Buro = Model();
	Buro.LoadModel("Models/Buro.obj");

	BuroPuerta = Model();
	BuroPuerta.LoadModel("Models/BuroPuerta.obj");

	BuroCajon = Model();
	BuroCajon.LoadModel("Models/BuroCajon.obj");

	Silla = Model();
	Silla.LoadModel("Models/Silla.obj");

	AsientoSilla = Model();
	AsientoSilla.LoadModel("Models/AsientoSilla.obj");

	////Planta Alta
	SillDobl = Model();
	SillDobl.LoadModel("Models/SillDobl.obj");

	LamPared = Model();
	LamPared.LoadModel("Models/LamPared.obj");

	Mesa = Model();
	Mesa.LoadModel("Models/Mesa.obj");

	AsBikeSup = Model();
	AsBikeSup.LoadModel("Models/AsBikeSup.obj");

	AsBike = Model();
	AsBike.LoadModel("Models/AsBike.obj");

	SillInd = Model();
	SillInd.LoadModel("Models/SillInd.obj");

	//Cornelias
	Casa = Model();
	Casa.LoadModel("Models/Casa.obj");

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
	/*mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.65f, 0.3f,
		0.0f, 0.0f, -1.0f);*/

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

	//Variables animacion Velocidad
	movOffset = 1.5f;
	movOffsetPuerta = 1.5f;
	movOffsetCajon = 0.01f;


	lastTime = glfwGetTime(); //Para empezar lo más cercano posible a 0

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Animaciones
		
		//Bici (1)
		if (mainWindow.getsilla() == -1.0f)
		{
			movBici += movOffset * deltaTime;
		}

		//Puerta (2)
		if (mainWindow.getpuerta() == -1.0f)
		{
			movPuerta += movOffsetPuerta * deltaTime;
		}

		//Cajon (3)

		if (mainWindow.getcajon() == -1.0f)
		{
			if (dirCajon == true)
			{
				movCajon -= movOffsetCajon * deltaTime;
				if (movCajon <= -0.0f)
				{
					dirCajon = false;
				}
			}
			else if (dirCajon == false)
			{
				movCajon += movOffsetCajon * deltaTime;
				if (movCajon >= 0.3f)
				{
					dirCajon = true;
				}
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

		
		

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		//**************************************************************************PISO**************************************************************************
		
		//Luz Exterior
		mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
			0.5f, 0.5f,
			0.0f, 0.0f, -1.0f);

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//**************************************************************************EDIFICACIONES**************************************************************************

		//Luz de casa
		mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
			0.5f, 0.05f,
			0.0f, 0.0f, -1.0f);

		shaderList[0].SetDirectionalLight(&mainLight);

		//Casa
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Casa.RenderModel();
		

		//**************************************************************************OBJETOS**************************************************************************

		
		//Luz de cuartos (Objetos)
		mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
			0.5f, 0.005f,
			0.0f, 0.0f, -1.0f);

		shaderList[0].SetDirectionalLight(&mainLight);

		//PLANTA ALTA
		//Poster Taylor (1)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(47.8f, 150.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Taylor.RenderModel();

		//Cama (2)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 112.0f, -59.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cama.RenderModel();

		//Jarron (3)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 134.5f, 91.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Jarron.RenderModel();

		//Buro (4) Jerarquia
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 115.0f, 91.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		modelaux = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Buro.RenderModel();

		//Buro Puerta (4) Jerarquia
		model = glm::translate(model, glm::vec3(0.75f, 0.1f, 0.2f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BuroPuerta.RenderModel();
		
		//Buro Cajon (4) Jerarquia
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.4f, 0.4f , 0.11f + movCajon));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BuroCajon.RenderModel();

		//Silla (5)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, 119.1f, 91.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		model = glm::rotate(model, -135 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Silla.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AsientoSilla.RenderModel();

		//PLANTA BAJA
		//Sillon Doble (6)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-23.0f, 10.35f, -5.0f));
		model = glm::scale(model, glm::vec3(20.0f, 30.0f, 30.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SillDobl.RenderModel();

		//Lampara Pared 1 (7)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.6f, 55.0f, -25.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		LamPared.RenderModel();

		//Lampara Pared 2 (7)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.6f, 55.0f, -25.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		LamPared.RenderModel();

		//Mesa de centro (8)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(23.0f, -0.5f, 30.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 15.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mesa.RenderModel();

		//Base Bicicleta (9) Jerarquia
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(23.0f, 1.8f, -5.0f));
		model = glm::scale(model, glm::vec3(7.0f, 10.0f, 7.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AsBike.RenderModel();

		//Asiento Silla Bicicleta (9) Jerarquia
		
		model = glm::translate(model, glm::vec3(-0.0f, 0.0f, -0.0f));
		model = glm::rotate(model, movBici * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, movZigZag * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));


		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AsBikeSup.RenderModel();

		//Sillon Individual (10)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 11.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, -135 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SillInd.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
