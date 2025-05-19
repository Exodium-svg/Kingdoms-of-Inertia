#include "main.h"

//#define NoDebugger
int gHeight = 0;
int gWidth = 0;

bool resized = false;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	gWidth = width;
	gHeight = height;

	resized = true;
	glViewport(0, 0, width, height);
}

void Cleanup(HANDLE hMutex) {
	if (NULL != hMutex)
		CloseHandle(hMutex);

	glfwTerminate();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd) {
	HANDLE hMutex = CreateMutexA(NULL, FALSE, "KingdomsOfInertia");
	
	if ((HANDLE)ERROR_ALREADY_EXISTS == hMutex) {
		DebugPrint("KingdomsOfInertia is already running");
		Cleanup(hMutex);
		return -1;
	}

#ifdef NoDebugger
	if (IsDebuggerPresent()) {
		ConsolePrint("Debugger detected idiot rejected");
		Cleanup(hMutex);
		return -1;
	}
#endif

	GLFWwindow* pGlfwWindow;

	if (!glfwInit()) {
		DebugPrint("Failed to initialize GLFW");
		Cleanup(hMutex);
		return -1;
	}

#ifdef _DEBUG
	char bufTitle[] = "Kingdoms of Inertia (Debug)";
#else
	char bufTitle[] = "Kingdoms of Inertia";
#endif

	gWidth = 800;
	gHeight = 600;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	pGlfwWindow = glfwCreateWindow(gWidth, 600, bufTitle, nullptr, NULL);

	if (nullptr == pGlfwWindow) {
		DebugPrint("Failed to create GLFW window");
		Cleanup(hMutex);
		return -1;
	}

	glfwMakeContextCurrent(pGlfwWindow);
	glfwSetFramebufferSizeCallback(pGlfwWindow, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		DebugPrint("Failed to load OpenGL");
		Cleanup(hMutex);
		return -1;
	}

	glfwSwapBuffers(pGlfwWindow); // to let render doc know we done

	LARGE_INTEGER nFrequency;
	LARGE_INTEGER lastTime, currentTime;

	QueryPerformanceFrequency(&nFrequency);
	QueryPerformanceCounter(&currentTime);
	lastTime = currentTime;

	Renderer2D renderer2D(3024, gWidth, gHeight);
	
	Sprite* sprite = renderer2D.AllocateSprite(0, 0, 400, 600);
	//Sprite* sprite2 = renderer2D.AllocateSprite(0, 400, 200, 150);
	
	renderer2D.SetTexture(sprite, "test");
	//renderer2D.SetTexture(sprite2, "test");

	Text2D text = renderer2D.CreateText(0, 0, 1, "Hello world", 32);

	//sprite->SetZIndex(1);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//TODO: implement a buffer just for text and add to render step 

	while (!glfwWindowShouldClose(pGlfwWindow)) {
#ifdef NoDebugger
		if (IsDebuggerPresent())
			break;
#endif
		glfwPollEvents();
		QueryPerformanceCounter(&currentTime);
		
		DeltaTimeMS deltaTimeMs = (currentTime.QuadPart - lastTime.QuadPart) * 1000 / nFrequency.QuadPart;

		if (resized)
			renderer2D.Resize(gWidth, gHeight);

		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

		renderer2D.Update(deltaTimeMs);
		renderer2D.Draw();
		
		glfwSwapBuffers(pGlfwWindow);

#ifdef _DEBUG
		GLenum error = glGetError();

		while (GL_NO_ERROR != error) {
			throw GetGlError(error);
		}
#endif
	}

	glfwDestroyWindow(pGlfwWindow);
	Cleanup(hMutex);
	return 0;
}

