/*
Proyecto
*/
//para cargar imagen
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
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//Variables
float terceraSonic = 25.0f;				// Distancia de camara
float terceraDepresso = 20.0f;			// Distancia de camara

int numAni = 3;							//Animales por conjunto
float anguloEntreAni = 90.0f;			//Giro de animales

float distanciaEntrePollos = 2.0f;		//Espacio entre cada Pollo
float distanciaEntreOvejas = 4.0f;		//Espacio entre cada Oveja
float distanciaEntreCrab = 3.0f;		// Espacio entre cada crab
float distanciaEntreFrutas = -10.0f;	// Espacio entre cada fruta en el eje z
float distanciaEntreHongos = -5.0f;		// Espacio entre cada hongo en el eje x
float distanciaEntrePalmeras = -25.0f;	//Espacio entre cada palmera en el eje x

int numCrab = 6;						//6x6 = 36 crabmeats
int numFrutas = 13;						//Número de frutas a crear
int numPalmeras = 27;					//Número de palmeras a crear calle
int numPalmerasAltar = 5;				//5*5 = 25 Número de palmeras a crear Altar
int numHongos = 92;						// Número de Hongos a crear

//Variables animacion 
//Animacion compleja Orbe
float movZigZag = 0.0f;					//Movimiento en el eje z de Orbe
float movVert = 0.0f;					//Movimiento en el eje y de Orbe
float movOffset;
bool dir = true;						//Para cambiar la direccion en el eje y

//Animacion Vocho
float movVoch;							//Movimiento en el eje x de Vocho
float movVochOffset;
float giraVoch;							//Para dar un giro de 180°
float rotllVoch;						//Para las llantas
float rotllVochOffset;
bool dirVoch = true;					//Para cambiar la direccion en el eje x

//Animacion Bus
float movBus;							//Movimiento en el eje x de bus
float movBusOffset;
float giraBus;							//Para dar un giro de 180°
float rotllBus;							//Para las llantas
float rotllBusOffset;
bool dirBus = true;						//Para cambiar la direccion en el eje x

//Animacion Moto
float movMoto;							//Movimiento en el eje x de moto
float movMotoOffset;
float giraMoto;							//Para dar un giro de 180°
float rotllMoto;						//Para las llantas
float rotllMotoOffset;
bool dirMoto = true;					//Para cambiar la direccion en el eje x

//Animacion basica MotoBug
bool dirAnimBas;						//Direccion de motobug
float movAnimBas;						//Movimiento de motobug
float movAnimBasOffset = true;

//Animacion basica Mineral
float movAnimBas2;						//Movimiento de Mineral
float movAnimBas2Offset = true;

//Ciclos automaticos
//Ciclo automatico Luz Moto
float contaLuzMoto;						//Animacion de prendido y apagado Luz moto
float contaLuzMotoOffset;
bool prendeLuzMoto;						//Para definir si esta prendida o apagada

//Ciclo dia y noche
float solDirZ;							//Direccion en Z del sol
float solDirY;							//Direccion en Y del sol
float solDirYOffset;
bool invierteCiclo;						//Para cambiar entre noche y dia
bool esDeDia;							//Verificar si es de dia



Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Camaras
Camera camera;
Camera depresso;
Camera sonic;

//Texturas a utilizar en entorno opengl
Texture pisoTexture;	//Textura de piso
Texture banca1Texture;	//Textura de la banca

//Modelos a utilizar en entorno opengl

//Edificaciones
Model PSol;
Model PLuna;
Model Altar;

//Objetos
Model Lifmunk;
Model PalBall;
Model Mineral;
Model FruHa;
Model Orbe;
Model Pollo;
Model Oveja;

Model Motobug;
Model Crabmeat;
Model Palmera;
Model Checkpoint;
Model Joya;
Model Cofre;
Model CajaAnillo;
Model Oro;
Model Baculo;
Model HongoM;

//Vehiculos
//Vochito
Model VCha;
Model VCof;
Model VLlDDe;
Model VLlDIz;
Model VLlTDe;
Model VLlTIz;

//Autobus
Model ACha;
Model ALlDDe;
Model ALlDIz;
Model ALlTDe;
Model ALlTIz;

//Moto
Model MCha;
Model MLl;

//Personaje
//Depresso
Model DCu;
Model DBDe;
Model DBIz;
Model DPDe;
Model DPIz;

//Sonic
Model SCu;
Model SBDe;
Model SBIz;
Model SPDe;
Model SPIz;

//Skybox a utilizar en entorno opengl
//Dos tipos para el dia y la noche
Skybox skybox;
Skybox skybox2;

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

//Creacion de banca con primitivas OpenGl
void CrearBanca1()
{
	unsigned int banca_indices[] = {

		//LADO IZQUIERDO
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,

		//LADO DERECHO
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35,
		36, 37, 38,
		39, 40, 41,
		42, 43, 44,
		45, 46, 47,

		//PARTE DE ARRIBA
		48, 49, 50,
		51, 52, 53,
		54, 55, 56,
		57, 58, 59,
		60, 61, 62,
		63, 64, 65,
		66, 67, 68,
		69, 70, 71,

		72, 73, 74, //tapa de arriba
		75, 76, 77,
		78, 79, 80, //tapa de abajo
		81, 82, 83,
	};


	GLfloat banca_vertices[] = {
		// S y T se modifican para las coordenadas de la imagen que se mostrara 

		// LADO IZQUIERDO
		// CARA FRONTAL
		//x		y		z			S		T			NX		NY		NZ
		-4.0f,	0.0f,	1.0f,		0.0f,   0.0f,		0.0f,	0.0f,	-1.0f,	//0
		-3.0f,	0.0f,	1.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		-4.0f,	3.0f,	1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2

		-3.0f,	0.0f,	1.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		-4.0f,	3.0f,	1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		-3.0f,	3.0f,	1.0f,	    1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3

		// CARA IZQUIERDA
		-4.0f,	0.0f,	1.0f,		0.0f,   1.0f,		1.0f,	0.0f,	0.0f,	//0
		-4.0f,	0.0f,	-1.0f,		0.0f,   0.0f,		1.0f,	0.0f,	0.0f,	//4
		-4.0f,	3.0f,	-1.0f,		1.0f,	0.0f,		1.0f,	0.0f,	0.0f,	//6

		-4.0f,	3.0f,	-1.0f,		1.0f,	0.0f,		1.0f,	0.0f,	0.0f,	//6
		-4.0f,	3.0f,	1.0f,		1.0f,	1.0f,		1.0f,	0.0f,	0.0f,	//2
		-4.0f,	0.0f,	1.0f,		0.0f,   1.0f,		1.0f,	0.0f,	0.0f,	//0

		// CARA DERECHA
		-3.0f,	0.0f,	1.0f,	    1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//1
		-3.0f,	0.0f,	-1.0f,	    0.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//5
		-3.0f,	3.0f,	-1.0f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,	//7

		-3.0f,	3.0f,	-1.0f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,	//7
		-3.0f,	3.0f,	1.0f,	    1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,	//3
		-3.0f,	0.0f,	1.0f,	    1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//1

		// CARA TRASERA
		-3.0f,	0.0f,	-1.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	1.0f,	//5
		-4.0f,	0.0f,	-1.0f,		0.0f,   0.0f,		0.0f,	0.0f,	1.0f,	//4
		-4.0f,	3.0f,	-1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	1.0f,	//6

		-4.0f,	3.0f,	-1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	1.0f,	//6
		-3.0f,	3.0f,	-1.0f,	    1.0f,	1.0f,		0.0f,	0.0f,	1.0f,	//7
		-3.0f,	0.0f,	-1.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	1.0f,	//5

		// LADO DERECHO
		// CARA FRONTAL
		//x		y		z			S		T			NX		NY		NZ
		4.0f,	0.0f,	1.0f,		0.0f,   0.0f,		0.0f,	0.0f,	-1.0f,	//0
		3.0f,	0.0f,	1.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		4.0f,	3.0f,	1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2

		3.0f,	0.0f,	1.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		4.0f,	3.0f,	1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		3.0f,	3.0f,	1.0f,	    1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3

		// CARA IZQUIERDA
		4.0f,	0.0f,	1.0f,		0.0f,   1.0f,		-1.0f,	0.0f,	0.0f,	//0
		4.0f,	0.0f,	-1.0f,		0.0f,   0.0f,		-1.0f,	0.0f,	0.0f,	//4
		4.0f,	3.0f,	-1.0f,		1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//6

		4.0f,	3.0f,	-1.0f,		1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//6
		4.0f,	3.0f,	1.0f,		1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,	//2
		4.0f,	0.0f,	1.0f,		0.0f,   1.0f,		-1.0f,	0.0f,	0.0f,	//0

		// CARA DERECHA
		3.0f,	0.0f,	1.0f,	    1.0f,	0.0f,		1.0f,	0.0f,	0.0f,	//1
		3.0f,	0.0f,	-1.0f,	    0.0f,	0.0f,		1.0f,	0.0f,	0.0f,	//5
		3.0f,	3.0f,	-1.0f,	    0.0f,	1.0f,		1.0f,	0.0f,	0.0f,	//7

		3.0f,	3.0f,	-1.0f,	    0.0f,	1.0f,		1.0f,	0.0f,	0.0f,	//7
		3.0f,	3.0f,	1.0f,	    1.0f,	1.0f,		1.0f,	0.0f,	0.0f,	//3
		3.0f,	0.0f,	1.0f,	    1.0f,	0.0f,		1.0f,	0.0f,	0.0f,	//1

		// CARA TRASERA
		3.0f,	0.0f,	-1.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	1.0f,	//5
		4.0f,	0.0f,	-1.0f,		0.0f,   0.0f,		0.0f,	0.0f,	1.0f,	//4
		4.0f,	3.0f,	-1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	1.0f,	//6

		4.0f,	3.0f,	-1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	1.0f,	//6
		3.0f,	3.0f,	-1.0f,	    1.0f,	1.0f,		0.0f,	0.0f,	1.0f,	//7
		3.0f,	0.0f,	-1.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	1.0f,	//5


		// PARTE SUPERIOR
		// CARA FRONTAL
		//x		y		z			S		T			NX		NY		NZ
		-5.0f,	3.0f,	2.0f,		0.0f,   0.0f,		0.0f,	0.0f,	-1.0f,	//0
		5.0f,	3.0f,	2.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		-5.0f,	4.0f,	2.0f,		0.0f,	0.2f,		0.0f,	0.0f,	-1.0f,	//2

		5.0f,	3.0f,	2.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		-5.0f,	4.0f,	2.0f,		0.0f,	0.2f,		0.0f,	0.0f,	-1.0f,	//2
		5.0f,	4.0f,	2.0f,	    1.0f,	0.2f,		0.0f,	0.0f,	-1.0f,	//3

		// CARA IZQUIERDA
		-5.0f,	3.0f,	2.0f,		0.0f,   1.0f,		1.0f,	0.0f,	0.0f,	//0
		-5.0f,	3.0f,	-2.0f,		0.0f,   0.0f,		1.0f,	0.0f,	0.0f,	//4
		-5.0f,	4.0f,	-2.0f,		0.5f,	0.0f,		1.0f,	0.0f,	0.0f,	//6

		-5.0f,	4.0f,	-2.0f,		0.5f,	0.0f,		1.0f,	0.0f,	0.0f,	//6
		-5.0f,	4.0f,	2.0f,		0.5f,	1.0f,		1.0f,	0.0f,	0.0f,	//2
		-5.0f,	3.0f,	2.0f,		0.0f,   1.0f,		1.0f,	0.0f,	0.0f,	//0

		// CARA DERECHA
		5.0f,	3.0f,	2.0f,	    1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//1
		5.0f,	3.0f,	-2.0f,	    0.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//5
		5.0f,	4.0f,	-2.0f,	    0.0f,	0.5f,		-1.0f,	0.0f,	0.0f,	//7

		5.0f,	4.0f,	-2.0f,	    0.0f,	0.5f,		-1.0f,	0.0f,	0.0f,	//7
		5.0f,	4.0f,	2.0f,	    1.0f,	0.5f,		-1.0f,	0.0f,	0.0f,	//3
		5.0f,	3.0f,	2.0f,	    1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//1

		// CARA TRASERA
		5.0f,	3.0f,	-2.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	1.0f,	//5
		-5.0f,	3.0f,	-2.0f,		0.0f,   0.0f,		0.0f,	0.0f,	1.0f,	//4
		-5.0f,	4.0f,	-2.0f,		0.0f,	0.2f,		0.0f,	0.0f,	1.0f,	//6

		-5.0f,	4.0f,	-2.0f,		0.0f,	0.2f,		0.0f,	0.0f,	1.0f,	//6
		5.0f,	4.0f,	-2.0f,	    1.0f,	0.2f,		0.0f,	0.0f,	1.0f,	//7
		5.0f,	3.0f,	-2.0f,	    1.0f,	0.0f,		0.0f,	0.0f,	1.0f,	//5

		//TAPA SUPERIOR
		-5.0f,	4.0f,	2.0f,		0.0f,	0.0f,		0.0f,	-1.0f,	0.0f,	//2
		-5.0f,	4.0f,	-2.0f,		1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,	//6
		5.0f,	4.0f,	2.0f,	    0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,	//3

		5.0f,	4.0f,	-2.0f,	    1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,	//7
		-5.0f,	4.0f,	-2.0f,		1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,	//6
		5.0f,	4.0f,	2.0f,	    0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,	//3

		//TAPA INFERIOR 
		//(aunque en realidad no es tan necesaria a menos que se quiera ver la banca desde abajo)
		-5.0f,	3.0f,	2.0f,		0.0f,	0.0f,		0.0f,	1.0f,	0.0f,	//2
		-5.0f,	3.0f,	-2.0f,		1.0f,	0.0f,		0.0f,	1.0f,	0.0f,	//6
		5.0f,	3.0f,	2.0f,	    0.0f,	1.0f,		0.0f,	1.0f,	0.0f,	//3

		5.0f,	3.0f,	-2.0f,	    1.0f,	1.0f,		0.0f,	1.0f,	0.0f,	//7
		-5.0f,	3.0f,	-2.0f,		1.0f,	0.0f,		0.0f,	1.0f,	0.0f,	//6
		5.0f,	3.0f,	2.0f,	    0.0f,	1.0f,		0.0f,	1.0f,	0.0f,	//3
	};

	Mesh* banca1 = new Mesh();
	banca1->CreateMesh(banca_vertices, banca_indices, 768, 84);
	meshList.push_back(banca1);

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
	CrearBanca1();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 400.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), -90.0f, -90.0f, 0.3f, 0.5f);
	depresso = Camera(glm::vec3(-220.0f, 9.0f, 61.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);
	sonic = Camera(glm::vec3(201.0f, 10.0f, 27.0f), glm::vec3(0.0f, 1.0f, 0.0f), 180.0f, 0.0f, 0.3f, 0.5f);

	//********************************CARGA DE TEXTURAS*************************************
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	//Textura de la banca
	banca1Texture = Texture("Textures/piedra_banca.png");
	banca1Texture.LoadTextureA(); 

	//********************************CARGA DE MODELOS*************************************
	//Objetos
	Lifmunk = Model();
	Lifmunk.LoadModel("Models/Lifmunk.obj");
	
	PalBall = Model();
	PalBall.LoadModel("Models/PalBall.obj");

	Mineral = Model();
	Mineral.LoadModel("Models/Mineral.obj");

	Orbe = Model();
	Orbe.LoadModel("Models/Orbe.obj");

	Motobug = Model();
	Motobug.LoadModel("Models/ModelosAle/motobug.obj");

	Checkpoint = Model();
	Checkpoint.LoadModel("Models/ModelosAle/checkpoint.obj");

	Joya = Model();
	Joya.LoadModel("Models/ModelosAle/joya.obj");
	
	Cofre = Model();
	Cofre.LoadModel("Models/ModelosAle/cofre.obj");

	CajaAnillo = Model();
	CajaAnillo.LoadModel("Models/ModelosAle/ring_monitor.obj");

	Oro = Model();
	Oro.LoadModel("Models/ModelosAle/lingote_oro.obj");

	Baculo = Model();
	Baculo.LoadModel("Models/ModelosAle/baculo.obj");
	
	//FLORA
	FruHa = Model();
	FruHa.LoadModel("Models/FruHa.obj");

	Palmera = Model();
	Palmera.LoadModel("Models/ModelosAle/palmera.obj");

	HongoM = Model();
	HongoM.LoadModel("Models/ModelosAle/hongo_magico.obj");

	//FAUNA
	Crabmeat = Model();
	Crabmeat.LoadModel("Models/ModelosAle/crabmeat.obj");

	Pollo = Model();
	Pollo.LoadModel("Models/Pollo.obj");

	Oveja = Model();
	Oveja.LoadModel("Models/Oveja.obj");
	
	//Vehiculos
	//Vochito
	VCha = Model();
	VCha.LoadModel("Models/VCha.obj");

	VCof = Model();
	VCof.LoadModel("Models/VCof.obj");

	VLlDDe = Model();
	VLlDDe.LoadModel("Models/VLlDDe.obj");

	VLlDIz = Model();
	VLlDIz.LoadModel("Models/VLlDIz.obj");

	VLlTDe = Model();
	VLlTDe.LoadModel("Models/VLlTDe.obj");

	VLlTIz = Model();
	VLlTIz.LoadModel("Models/VLlTIz.obj");

	//Autobus
	ACha = Model();
	ACha.LoadModel("Models/ACha.obj");

	ALlDDe = Model();
	ALlDDe.LoadModel("Models/ALlDDe.obj");

	ALlDIz = Model();
	ALlDIz.LoadModel("Models/ALlDIz.obj");

	ALlTDe = Model();
	ALlTDe.LoadModel("Models/ALlTDe.obj");

	ALlTIz = Model();
	ALlTIz.LoadModel("Models/ALlTIz.obj");

	//Moto
	MCha = Model();
	MCha.LoadModel("Models/ModelosAle/moto_cuerpo.obj");

	MLl = Model();
	MLl.LoadModel("Models/ModelosAle/moto_llanta.obj");

	//Personajes
	//Depresso
	DCu = Model();
	DCu.LoadModel("Models/DCu.obj");

	DBDe = Model();
	DBDe.LoadModel("Models/DBDe.obj");

	DBIz = Model();
	DBIz.LoadModel("Models/DBIz.obj");

	DPDe = Model();
	DPDe.LoadModel("Models/DPDe.obj");

	DPIz = Model();
	DPIz.LoadModel("Models/DPIz.obj");

	//Sonic
	SCu = Model();
	SCu.LoadModel("Models/ModelosAle/sonic_cuerpo.obj");

	SBDe = Model();
	SBDe.LoadModel("Models/ModelosAle/sonic_brazo_derecho.obj");

	SBIz = Model();
	SBIz.LoadModel("Models/ModelosAle/sonic_brazo_izquierdo.obj");

	SPDe = Model();
	SPDe.LoadModel("Models/ModelosAle/sonic_pierna_derecha.obj");

	SPIz = Model();
	SPIz.LoadModel("Models/ModelosAle/sonic_pierna_izquierda.obj");

	//Edificaciones
	PSol = Model();
	PSol.LoadModel("Models/PSol.obj");

	PLuna = Model();
	PLuna.LoadModel("Models/ModelosAle/PLuna.obj");
	
	Altar = Model();
	Altar.LoadModel("Models/ModelosAle/Altar.obj");

	std::vector<std::string> skyboxFaces;
	std::vector<std::string> skyboxFaces2;

	skyboxFaces.push_back("Textures/Skybox/skybox_3.tga"); //right
	skyboxFaces.push_back("Textures/Skybox/skybox_1.tga"); //left
	skyboxFaces.push_back("Textures/Skybox/skybox_6.tga"); //down
	skyboxFaces.push_back("Textures/Skybox/skybox_5.tga"); //up
	skyboxFaces.push_back("Textures/Skybox/skybox_2.tga"); //front
	skyboxFaces.push_back("Textures/Skybox/skybox_4.tga"); //bh

	skyboxFaces2.push_back("Textures/Skybox/skybox_3_noche.tga"); //right
	skyboxFaces2.push_back("Textures/Skybox/skybox_1_noche.tga"); //left
	skyboxFaces2.push_back("Textures/Skybox/skybox_6_noche.tga"); //down
	skyboxFaces2.push_back("Textures/Skybox/skybox_5_noche.tga"); //up
	skyboxFaces2.push_back("Textures/Skybox/skybox_2_noche.tga"); //front
	skyboxFaces2.push_back("Textures/Skybox/skybox_4_noche.tga"); //bh

	skybox = Skybox(skyboxFaces);
	skybox2 = Skybox(skyboxFaces2);

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
	
	//Variables para animacion de vehiculos
	movVoch=0.0f;
	movVochOffset = 0.7f;
	rotllVoch = 0.0f;
	rotllVochOffset = 3.5f;

	movMoto = 0.0f;
	movMotoOffset = 0.7f;
	rotllMoto = 0.0f;
	rotllMotoOffset = 3.5f;

	movBus = 0.0f;
	movBusOffset = 0.7f;
	rotllBus = 0.0f;
	rotllBusOffset = 3.5f;

	//Variable animacion Orbe
	movOffset = 0.05f;

	//Variables para el movimiento de los motobugs y mineral
	dirAnimBas = true;
	movAnimBas = 0.0f;
	movAnimBasOffset = 0.05f;

	movAnimBas2 = 0.0f;
	movAnimBas2Offset = 0.5f;

	//Apagado y prendido automático de la luz de la moto
	contaLuzMoto = 0.0f;
	contaLuzMotoOffset = 1.0;
	prendeLuzMoto = true;

	//ciclo de dia y de noche
	solDirZ = -1.0f;
	solDirY = 0.0f;
	solDirYOffset = 0.1f;
	invierteCiclo = true;
	esDeDia = true;


	lastTime = glfwGetTime(); //Para empezar lo más cercano posible a 0

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//luz del sol
		if (esDeDia == true) { //es de día 
			if (invierteCiclo == true)
			{
				solDirY -= solDirYOffset * deltaTime;
				solDirZ = -1 - (solDirY / 100);
				if (solDirY <= -100.0f)
				{
					invierteCiclo = false;
				}
			}
			else
			{
				solDirY += solDirYOffset * deltaTime;
				solDirZ = (-1 - (solDirY / 100)) * -1;
				if (solDirY >= 0.0f)
				{
					invierteCiclo = true;
					esDeDia = false;
				}
			}
		}
		else { //es de noche
			if (invierteCiclo == true)
			{
				solDirY += solDirYOffset * deltaTime;
				solDirZ = 1 - (solDirY / 100);
				if (solDirY >= 100.0f)
				{
					invierteCiclo = false;
				}
			}
			else
			{
				solDirY -= solDirYOffset * deltaTime;
				solDirZ = (1 - (solDirY / 100)) * -1;
				if (solDirY <= 0.0f)
				{
					invierteCiclo = true;
					esDeDia = true;
				}
			}
		}


		//Algoritmos de animacion
		// Animacion de los motobugs
		if (dirAnimBas == true)
		{
			movAnimBas += movAnimBasOffset * deltaTime;

			if (movAnimBas > 15.0f)
			{
				dirAnimBas = false;
			}
		}
		else if (dirAnimBas == false)
		{
			movAnimBas -= movAnimBasOffset * deltaTime;
			if (movAnimBas < 0.0f)
			{
				dirAnimBas = true;
			}
		}
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
		
		//Animacion Vehiculos
		//Vocho
		if (dirVoch == true)
		{
			movVoch -= movVochOffset * deltaTime;
			rotllVoch += rotllVochOffset * deltaTime;
			if (movVoch <= -130.0f)
			{
					dirVoch = false;
					giraVoch = 180.0f;
			}
		}
		else
		{
			movVoch += movVochOffset * deltaTime;
			rotllVoch += rotllVochOffset * deltaTime;
			if (movVoch >= 160.0f)
			{
					dirVoch = true;
					giraVoch = 0.0f;
			}
		}
		//Moto
		if (dirMoto == true)
		{
			movMoto += movMotoOffset * deltaTime;
			rotllMoto += rotllMotoOffset * deltaTime;
			if (movMoto >= 100.0f)
			{
				dirMoto = false;
				giraMoto = 180.0f;
			}
		}
		else
		{
			movMoto -= movMotoOffset * deltaTime;
			rotllMoto += rotllMotoOffset * deltaTime;
			if (movMoto <= -150.0f)
			{
				dirMoto = true;
				giraMoto = 0.0f;
			}
		}
		//Bus
		if (dirBus == true)
		{
			movBus -= movBusOffset * deltaTime;
			rotllBus += rotllBusOffset * deltaTime;
			if (movBus <= -320.0f)
			{
				dirBus = false;
				giraBus = 180.0f;
			}
		}
		else
		{
			movBus += movBusOffset * deltaTime;
			rotllBus += rotllBusOffset * deltaTime;
			if (movBus >= 250.0f)
			{
				dirBus = true;
				giraBus = 0.0f;
			}
		}

		//luz moto
		if (prendeLuzMoto == true)
		{
			contaLuzMoto -= contaLuzMotoOffset * deltaTime;
			if (contaLuzMoto <= -100.0f)
			{
				prendeLuzMoto = false;
			}
		}
		else
		{
			contaLuzMoto += contaLuzMotoOffset * deltaTime;
			if (contaLuzMoto >= 0.0f)
			{
				prendeLuzMoto = true;
			}
		}
		
		//Recibir eventos del usuario
		glfwPollEvents();

		//Camaras y controles asignados
		if (mainWindow.getopcion() == 0.0f)
		{
			depresso.keyControlDep(mainWindow.getsKeys(), deltaTime);
			depresso.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			
		}
		else if (mainWindow.getopcion() == 1.0f)
		{
			sonic.keyControlSon(mainWindow.getsKeys(), deltaTime);
			sonic.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else if (mainWindow.getopcion() == 2.0f)
		{
			//Para solo mover con teclado sin mouse
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
		}
		
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Para decidir que ven las camaras
		if (esDeDia) {
			if (mainWindow.getopcion() == 0.0f)
			{
				skybox.DrawSkybox(depresso.calculateViewMatrix(), projection);
			}
			else if (mainWindow.getopcion() == 1.0f)
			{
				skybox.DrawSkybox(sonic.calculateViewMatrix(), projection);
			}
			else if (mainWindow.getopcion() == 2.0f)
			{
				skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
			}
		}
		else {
			if (mainWindow.getopcion() == 0.0f)
			{
				skybox2.DrawSkybox(depresso.calculateViewMatrix(), projection);
			}
			else if (mainWindow.getopcion() == 1.0f)
			{
				skybox2.DrawSkybox(sonic.calculateViewMatrix(), projection);
			}
			else if (mainWindow.getopcion() == 2.0f)
			{
				skybox2.DrawSkybox(camera.calculateViewMatrix(), projection);
			}
		}
		

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

		//Cambiar entre camaras y guardar su posicion
		if (mainWindow.getopcion() == 0.0f)
		{
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(depresso.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, depresso.getCameraPosition().x, depresso.getCameraPosition().y, depresso.getCameraPosition().z);
		}
		else if (mainWindow.getopcion() == 1.0f)
		{
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(sonic.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, sonic.getCameraPosition().x, sonic.getCameraPosition().y, sonic.getCameraPosition().z);
		}
		else if (mainWindow.getopcion() == 2.0f)
		{
			
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}

		//Movimiento de las luces
		//Luces Bus
		if (dirBus) {
			spotLights[0].SetFlash(glm::vec3(movBus - 169.0f, 3.0f, 16.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		}
		else {
			spotLights[0].SetFlash(glm::vec3(movBus - 109.0f, 3.0f, 16.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		//Luces del Vocho
		if (dirVoch) {
			spotLights[1].SetFlash(glm::vec3(movVoch - 366.0f, 3.0f, -42.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		}
		else {
			spotLights[1].SetFlash(glm::vec3(movVoch - 346.0f, 3.0f, -42.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		
		//Luces la moto
		if (dirMoto) {
			spotLights[2].SetFlash(glm::vec3(movMoto + 47.0f, 3.6f, -40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else {
			spotLights[2].SetFlash(glm::vec3(movMoto + 47.0f, 3.6f, -40.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		}

		//Establecer direccion del sol 
		mainLight.SetDir(glm::vec3(0.0f, solDirY / 100, solDirZ));

		//movimiento de la luz del orbe
		pointLights[2].SetPLPos(glm::vec3(479.0f, 4.0f + movVert, -22.0f + 3.5*sin(glm::radians(movZigZag))));

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		if (mainWindow.getOrbePrendido()) {
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		else {
			shaderList[0].SetPointLights(pointLights, pointLightCount-1);
		}

		if (prendeLuzMoto == true) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else {
			shaderList[0].SetSpotLights(spotLights, spotLightCount-1);
		}
		

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

		//PIRAMIDE DE LA LUNA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(380.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		PLuna.RenderModel();
		glDisable(GL_BLEND);

		//ALTAR SOL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-250.0f, 1.0f, -270.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Altar.RenderModel();

		//ALTAR LUNA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 1.0f, -270.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Altar.RenderModel();

		//**************************************************************************PERSONAJES**************************************************************************
		
		//DEPRESSO
		//Cuerpo
		glm::vec3 camaraDepresso = depresso.getCameraDirection();
		glm::vec3 depressoPos = depresso.getCameraPosition() + terceraDepresso * camaraDepresso;
		depressoPos.y -= 6.0f;

		model = glm::mat4(1.0f);
		model = glm::translate(model, depressoPos);
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getgiroIzDepress()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getgiroDeDepress()), glm::vec3(0.0f, 1.0f, 0.0f));

		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DCu.RenderModel();

		//Brazo Izquierdo
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.8f, 0.6f, 0.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getmovDepress()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getmovDepress()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DBIz.RenderModel();

		//Brazo Derecho
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.6f, 0.6f, 0.9f));
		model = glm::rotate(model, glm::radians(-mainWindow.getmovDepress()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getmovDepress()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DBDe.RenderModel();

		//Pierna Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.4f, -0.6f, 0.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getmovDepress()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getmovDepress()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DPIz.RenderModel();

		//Pierna Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.4f, -0.8f, 0.8f));
		model = glm::rotate(model, glm::radians(-mainWindow.getmovDepress()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getmovDepress()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DPDe.RenderModel();

		//SONIC
		//Cuerpo
		glm::vec3 camaraSonic = sonic.getCameraDirection();
		glm::vec3 sonicPos = sonic.getCameraPosition() + terceraSonic * camaraSonic;
		sonicPos.y -= 8.0f;

		model = glm::mat4(1.0);
		model = glm::translate(model, sonicPos);
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getgiroIzSonic()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getgiroDeSonic()), glm::vec3(0.0f, 1.0f, 0.0f));

		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SCu.RenderModel(); //variable con el modelo del cuerpo de la moto
		
		//Brazo izquierdo
		model = modelaux; //reinicia la matriz auxiliar para la jerarquia
		model = glm::translate(model, glm::vec3(0.1f, 0.4f, 0.8f));
		model = glm::rotate(model, glm::radians(-mainWindow.getmovSonic()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getmovSonic()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SBIz.RenderModel();

		//Brazo derecho
		model = modelaux; //reinicia la matriz auxiliar para la jerarquia
		model = glm::translate(model, glm::vec3(0.1f, 0.4f, -0.7f));
		model = glm::rotate(model, glm::radians(mainWindow.getmovSonic()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getmovSonic()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SBDe.RenderModel();

		//Pierna izquierda
		model = modelaux; //reinicia la matriz auxiliar para la jerarquia
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.3f));
		model = glm::rotate(model, glm::radians(mainWindow.getmovSonic()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getmovSonic()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SPIz.RenderModel();

		//Pierna derecha
		model = modelaux; //reinicia la matriz auxiliar para la jerarquia
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -0.3f));
		model = glm::rotate(model, glm::radians(-mainWindow.getmovSonic()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getmovSonic()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SPDe.RenderModel();

		//**************************************************************************AUTOMOVILES**************************************************************************

		//VOCHITO
		//Chasis
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movVoch-356.0f, 3.0f, -42.0f));
		model = glm::rotate(model, glm::radians(giraVoch), glm::vec3(0.0f, 1.0f, 0.0f));
		
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		modelaux = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		VCha.RenderModel();

		//Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(-5.95f, 2.8f, -0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		VCof.RenderModel();

		//Llanta Delantera Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-11.5f, -4.3f, 7.3f));
		model = glm::rotate(model, rotllVoch * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		VLlDIz.RenderModel();

		//Llanta Delantera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-11.5f, -4.3f, -7.5f));
		model = glm::rotate(model, -rotllVoch * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		VLlDDe.RenderModel();

		//Llanta Trasera Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(14.0f, -4.3f, 7.0f));
		model = glm::rotate(model, rotllVoch * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		VLlTIz.RenderModel();

		//Llanta Trasera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(14.0f, -4.3f, -7.0f));
		model = glm::rotate(model, -rotllVoch * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		VLlTDe.RenderModel();

		//AUTOBUS
		//Chasis
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movBus - 139.0f, 4.0f, 16.0f));
		model = glm::rotate(model, glm::radians(giraBus), glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(5.5f, 5.5f, 5.5f));
		modelaux = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ACha.RenderModel();

		//Llanta Delantera Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.43f, -0.55f, 0.9f));
		model = glm::rotate(model, rotllBus * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ALlDIz.RenderModel();

		//Llanta Delantera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.43f, -0.55f, -0.8f));
		model = glm::rotate(model, rotllBus * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ALlDDe.RenderModel();

		//Llanta Trasera Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.43f, -0.55f, 0.9f));
		model = glm::rotate(model, rotllBus * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ALlTIz.RenderModel();

		//Llanta Trasera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.43f, -0.55f, -0.8f));
		model = glm::rotate(model, rotllBus * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ALlTDe.RenderModel();

		//MOTO
		//Chasis
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movMoto+47.0f, -1.0f, -40.0f));		
		model = glm::rotate(model, glm::radians(giraMoto-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MCha.RenderModel(); //variable con el modelo del cuerpo de la moto
		modelaux = model;

		//Llanta Delantera
		model = modelaux;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, -2.4f)); //mueve la llanta hacia abajo
		model = glm::rotate(model, -rotllMoto * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MLl.RenderModel(); //variable con el modelo de una sola llanta

		//Llanta trasera
		model = modelaux;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 2.4f)); //mueve la llanta hacia abajo
		model = glm::rotate(model, -rotllMoto * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MLl.RenderModel();

		//*********************************************************************FAUNA**************************************************************************
		
		//Pollos
		//Conjunto 1
		for (int i = 0; i < numAni; ++i) {

			for (int j = 0; j < numAni; ++j) {
				model = glm::mat4(1.0);
				//Ubicacion Oveja inicial
				glm::vec3 polloPos = glm::vec3(110.0f, 0.2f, 138.0f);

				//Ubicacion de Ovejas izquierda
				polloPos.x = polloPos.x + i * distanciaEntrePollos;
				polloPos.z = polloPos.z + j * distanciaEntrePollos;

				model = glm::translate(model, polloPos);
				model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
				model = glm::rotate(model, (anguloEntreAni * j) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, (anguloEntreAni * i) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Pollo.RenderModel();
			}
		}
		//Conjunto 2
		for (int i = 0; i < numAni; ++i) {

			for (int j = 0; j < numAni; ++j) {
				model = glm::mat4(1.0);
				//Ubicacion Pollo inicial
				glm::vec3 polloPos = glm::vec3(283.0f, 0.2f, 344.0f);

				//Ubicacion de Pollo izquierda
				polloPos.x = polloPos.x + i * distanciaEntrePollos;
				polloPos.z = polloPos.z + j * distanciaEntrePollos;

				model = glm::translate(model, polloPos);
				model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
				model = glm::rotate(model, (anguloEntreAni * j) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, (anguloEntreAni * i) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Pollo.RenderModel();
			}
		}
		
		//Ovejas
		//Conjunto 1
		for (int i = 0; i < numAni; ++i) {

			for (int j = 0; j < numAni; ++j) {
				model = glm::mat4(1.0);
				//Ubicacion Oveja inicial
				glm::vec3 ovejaPos = glm::vec3(-454.0f, 0.37f, 95.0f);

				//Ubicacion de Ovejas izquierda
				ovejaPos.x = ovejaPos.x + i * distanciaEntreOvejas;
				ovejaPos.z = ovejaPos.z + j * distanciaEntreOvejas;

				model = glm::translate(model, ovejaPos);
				model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
				model = glm::rotate(model, (anguloEntreAni * j) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, (anguloEntreAni * i) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Oveja.RenderModel();
			}
		}
		//Conjunto 2
		for (int i = 0; i < numAni; ++i) {

			for (int j = 0; j < numAni; ++j) {
				model = glm::mat4(1.0);
				//Ubicacion Oveja inicial
				glm::vec3 ovejaPos = glm::vec3(143.0f, 0.37f, -138.0f);

				//Ubicacion de Ovejas izquierda
				ovejaPos.x = ovejaPos.x + i * distanciaEntreOvejas;
				ovejaPos.z = ovejaPos.z + j * distanciaEntreOvejas;

				model = glm::translate(model, ovejaPos);
				model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
				model = glm::rotate(model, (anguloEntreAni * j) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, (anguloEntreAni * i) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Oveja.RenderModel();
			}
		}

		//Crabmeat
		//Conjunto 1
		for (int i = 0; i < numCrab; ++i) {

			for (int j = 0; j < numCrab; ++j) {
				//crabs 1
				model = glm::mat4(1.0);
				//Ubicacion de crab inicial
				glm::vec3 crabPos = glm::vec3(480.0f, -0.5f, -29.5f);
				
				//Ubicacion de siguientes crabs
				crabPos.x = crabPos.x + i * distanciaEntreCrab;
				crabPos.z = crabPos.z + j * distanciaEntreCrab;

				model = glm::translate(model, crabPos);
				model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
				model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Crabmeat.RenderModel();
			}
		}
		//Conjunto 2
		for (int i = 0; i < numCrab+5; ++i) {

			for (int j = 0; j < numCrab; ++j) {
				//crabs 1
				model = glm::mat4(1.0);
				//Ubicacion de crab inicial
				glm::vec3 crabPos = glm::vec3(-260.0f, -0.5f, 328.0f);

				//Ubicacion de siguientes crabs
				crabPos.x = crabPos.x + i * distanciaEntreCrab;
				crabPos.z = crabPos.z + j * distanciaEntreCrab;

				model = glm::translate(model, crabPos);
				model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Crabmeat.RenderModel();
			}
		}

		//**************************************************************************FLORA**************************************************************************

		//FrutasHabilidad
		for (int i = 0; i < numFrutas; ++i) {
			
			//Frutas Izquierda Camino
			model = glm::mat4(1.0);
			//Ubicacion de Fruta inicial
			glm::vec3 frutaPos = glm::vec3(365.0f, 0.2f, 245.0f);

			//Ubicacion de siguientes hongos
			frutaPos.z = frutaPos.z + i * distanciaEntreFrutas;

			model = glm::translate(model, frutaPos);
			model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			FruHa.RenderModel();

			if(i>2)
			{
				//Frutas Izquierda Camino
				model = glm::mat4(1.0);
				//Ubicacion de Fruta inicial
				glm::vec3 frutaPos = glm::vec3(330.0f, 0.2f, 245.0f);

				//Ubicacion de siguientes hongos
				frutaPos.z = frutaPos.z + i * distanciaEntreFrutas;

				model = glm::translate(model, frutaPos);
				model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				FruHa.RenderModel();
			}
		}

		//Palmeras
		for (int i = 0; i < numPalmeras; ++i) {
			//Palmeras Cruzando Calle
			model = glm::mat4(1.0);
			//Ubicacion de Palmera inicial
			glm::vec3 palPos = glm::vec3(175.0f, -0.75f, -83.0f);

			//Ubicacion de siguientes palmeras
			palPos.x = palPos.x + i * distanciaEntrePalmeras;


			model = glm::translate(model, palPos);
			model = glm::scale(model, glm::vec3(10.5f, 10.5f, 10.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Palmera.RenderModel();
			
			//Palmeras Antes Calle 
			if (i < 17)
			{
				model = glm::mat4(1.0);
				//Ubicacion de Palmera inicial
				glm::vec3 palPos = glm::vec3(175.0f, -0.75f, 62.0f);

				//Ubicacion de siguientes palmeras
				palPos.x = palPos.x + i * distanciaEntrePalmeras;

				model = glm::translate(model, palPos);
				model = glm::scale(model, glm::vec3(10.5f, 10.5f, 10.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Palmera.RenderModel();
			}
			else if (i > 17)
			{
				//Se quita la 18 y se continua con la 19
				model = glm::mat4(1.0);
				//Ubicacion de Palmera inicial
				glm::vec3 palPos = glm::vec3(175.0f, -0.75f, 62.0f);

				//Ubicacion de siguientes palmeras
				palPos.x = palPos.x + i * distanciaEntrePalmeras;

				model = glm::translate(model, palPos);
				model = glm::scale(model, glm::vec3(10.5f, 10.5f, 10.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Palmera.RenderModel();
			}
		}

		//Palmeras Atras altares
		//Conjunto 1
		for (int i = 0; i < numPalmerasAltar+35; ++i) {

			for (int j = 0; j < numPalmerasAltar; ++j) {
				model = glm::mat4(1.0);
				//Ubicacion de Palmera inicial
				glm::vec3 palPos = glm::vec3(-493.0f, -0.75f, -489.0f);

				//Ubicacion de siguientes palmeras
				palPos.x = palPos.x + i * (-distanciaEntrePalmeras);
				palPos.z = palPos.z + j * (-distanciaEntrePalmeras);

				model = glm::translate(model, palPos);
				model = glm::scale(model, glm::vec3(10.5f, 10.5f, 10.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Palmera.RenderModel();
			}
		}

		//Palmeras Atras Piramide Sol
		//Conjunto 2
		for (int i = 0; i < numPalmerasAltar + 35; ++i) {

			for (int j = 0; j < numPalmerasAltar; ++j) {
				model = glm::mat4(1.0);
				//Ubicacion de Palmera inicial
				glm::vec3 palPos = glm::vec3(-493.0f, -0.75f, 489.0f);

				//Ubicacion de siguientes palmeras
				palPos.x = palPos.x + i * (-distanciaEntrePalmeras);
				palPos.z = palPos.z + j * distanciaEntrePalmeras;

				model = glm::translate(model, palPos);
				model = glm::scale(model, glm::vec3(10.5f, 10.5f, 10.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Palmera.RenderModel();
			}
		}

		//Hongos
		for (int i = 0; i < numHongos; ++i) {
			
			//Hongos Antes Camino
			model = glm::mat4(1.0);
			//Ubicacion de Hongo inicial
			glm::vec3 hongoPosB = glm::vec3(360.0f, -1.0f, 253.1f);

			//Ubicacion de siguientes hongos
			hongoPosB.x = hongoPosB.x + i * distanciaEntreHongos;

			model = glm::translate(model, hongoPosB);
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			HongoM.RenderModel();

			if (i>5)  
			{
				//Hongos Cruzando Camino
				model = glm::mat4(1.0);
				//Ubicacion de Hongo inicial
				glm::vec3 hongoPos = glm::vec3(360.0f, -1.0f, 221.0f);

				//Ubicacion de siguientes hongos
				hongoPos.x = hongoPos.x + i * distanciaEntreHongos;

				model = glm::translate(model, hongoPos);
				model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				HongoM.RenderModel();
			}

		}

		//**************************************************************************OBJETOS**************************************************************************

		//Mineral (1)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-248.0f, 29.3f, 300.0f));
		model = glm::rotate(model, movAnimBas2 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mineral.RenderModel();

		//Lifmunk (2)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-147.0f, -1.0f, 62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lifmunk.RenderModel();

		//Cofre (3)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-84.0f, -1.0f, 164.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre.RenderModel();

		//Motobug 1 Piramide Luna (4)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(279.0f, -1.0f, -208.0f+movAnimBas));
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Motobug.RenderModel();

		//Motobug 2 Pasando Calle
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-333.0f, -1.0f, -120.0f + movAnimBas));
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Motobug.RenderModel();

		//Motobug 3 Calle
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(6.0f, -1.0f, 80.0f + movAnimBas));
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Motobug.RenderModel();

		//PalBall Cruzando Calle (5)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-290.0f, 0.45f, -181.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PalBall.RenderModel();

		//PalBall Calle
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(148.0f, 0.45f, 78.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PalBall.RenderModel();

		//Orbe Magico  (6)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(479.0f, 4.0f+movVert, -22.0f + 3.5*sin(glm::radians(movZigZag))));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Orbe.RenderModel();

		//Checkpoint  (7)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(143.0f, -0.0f, 237.0f));
		model = glm::scale(model, glm::vec3(13.0f, 13.0f, 13.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Checkpoint.RenderModel();

		//Joya  (8)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(244.0f, -0.0f, -114.0f)); 
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Joya.RenderModel();
		glDisable(GL_BLEND);

		//Caja anillo  (9)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-77.0f, -0.0f, 275.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CajaAnillo.RenderModel();

		//Oro (10)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-417.0f, -0.0f, 232.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Oro.RenderModel();

		//Baculo (11)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(305.0f, -1.0f, -75.7f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, 33 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Baculo.RenderModel();
		
		//Banca (12)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(201.0f, 0.0f, -58.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		banca1Texture.UseTexture();

		meshList[3]->RenderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
