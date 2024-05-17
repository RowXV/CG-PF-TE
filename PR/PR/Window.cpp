#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	giroIzDepress = 0.0f;
	giroDeDepress = 0.0f;
	movDepress = 0.0f;
	giroIzSonic = 0.0f;
	giroDeSonic = 0.0f;
	movSonic = 0.0f;
	opcion = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "PROYECTO", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	//DEPRESSO
	//Giros Depresso
	if (key == GLFW_KEY_Q)
	{
		theWindow->giroIzDepress += 10.0;
	}
	if (key == GLFW_KEY_E)
	{
		theWindow->giroDeDepress -= 10.0;
	}
	//Movimiento Depresso
	if (key == GLFW_KEY_W)
	{
		static bool aumentando = true;
		if (aumentando) {
			// Si está aumentando y no ha alcanzado el límite superior, aumenta
			if (theWindow->movDepress < 30.0) {
				theWindow->movDepress += 5.0;
			}
			else {
				// Si alcanza el límite superior, cambia la dirección
				aumentando = false;
			}
		}
		else {
			// Si está disminuyendo y no ha alcanzado el límite inferior, disminuye
			if (theWindow->movDepress > -30.0) {
				theWindow->movDepress -= 5.0;
			}
			else {
				// Si alcanza el límite inferior, cambia la dirección
				aumentando = true;
			}
		}
	}
	if (key == GLFW_KEY_S)
	{
		static bool aumentando = true;
		if (aumentando) {
			// Si está aumentando y no ha alcanzado el límite superior, aumenta
			if (theWindow->movDepress < 30.0) {
				theWindow->movDepress += 5.0;
			}
			else {
				// Si alcanza el límite superior, cambia la dirección
				aumentando = false;
			}
		}
		else {
			// Si está disminuyendo y no ha alcanzado el límite inferior, disminuye
			if (theWindow->movDepress > -30.0) {
				theWindow->movDepress -= 5.0;
			}
			else {
				// Si alcanza el límite inferior, cambia la dirección
				aumentando = true;
			}
		}
	}

	//SONIC
	//Giros Sonic
	if (key == GLFW_KEY_R)
	{
		theWindow->giroIzSonic += 10.0;
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow->giroDeSonic -= 10.0;
	}
	//Movimiento Sonic
	if (key == GLFW_KEY_T)
	{
		static bool aumentando = true;
		if (aumentando) {
			// Si está aumentando y no ha alcanzado el límite superior, aumenta
			if (theWindow->movSonic < 30.0) {
				theWindow->movSonic += 5.0;
			}
			else {
				// Si alcanza el límite superior, cambia la dirección
				aumentando = false;
			}
		}
		else {
			// Si está disminuyendo y no ha alcanzado el límite inferior, disminuye
			if (theWindow->movSonic > -30.0) {
				theWindow->movSonic -= 5.0;
			}
			else {
				// Si alcanza el límite inferior, cambia la dirección
				aumentando = true;
			}
		}
	}
	if (key == GLFW_KEY_G)
	{
		static bool aumentando = true;
		if (aumentando) {
			// Si está aumentando y no ha alcanzado el límite superior, aumenta
			if (theWindow->movSonic < 30.0) {
				theWindow->movSonic += 5.0;
			}
			else {
				// Si alcanza el límite superior, cambia la dirección
				aumentando = false;
			}
		}
		else {
			// Si está disminuyendo y no ha alcanzado el límite inferior, disminuye
			if (theWindow->movSonic > -30.0) {
				theWindow->movSonic -= 5.0;
			}
			else {
				// Si alcanza el límite inferior, cambia la dirección
				aumentando = true;
			}
		}
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS) //condicional para prender la luz del orbe
	{
		if (theWindow->orbePrendido == true) {
			theWindow->orbePrendido = false;
		}
		else {
			theWindow->orbePrendido = true;
		}
	}
	//Opcion Camara
	if (key == GLFW_KEY_1)
	{
		theWindow->opcion = 0.0;
	}
	if (key == GLFW_KEY_2)
	{
		theWindow->opcion = 1.0;
	}
	if (key == GLFW_KEY_3)
	{
		theWindow->opcion = 2.0;
	}


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
