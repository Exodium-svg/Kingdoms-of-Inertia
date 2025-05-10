#include "main.h"

//#define NoDebugger

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Camera::Resize(width, height);
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	pGlfwWindow = glfwCreateWindow(800, 600, bufTitle, nullptr, NULL);
	Camera::Resize(800, 600);
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

	UIManager manager(1024);
	ShaderProgram program = Shaders::CreateProgram("basic");
	

	UIElement * element = manager.AllocateElement(0, 0, 800, 300);
	UIElement* element2 = manager.AllocateElement(80, 0, 400, 600);

	//manager.SetSprite(element, "test");
	manager.SetSprite(element2, "test");

	int location = 0;
	while (!glfwWindowShouldClose(pGlfwWindow)) {
#ifdef NoDebugger
		if (IsDebuggerPresent())
			break;
#endif
		glfwPollEvents();
		QueryPerformanceCounter(&currentTime);
		
		// Move projection to UI manager.
		const glm::mat4x4& projection = Camera::GetProjection();
		Shaders::SetUniform(program, "projection", false, &projection);
		Shaders::SetUniform(program, "atlas", 0);

		DeltaTimeMS deltaTimeMs = (currentTime.QuadPart - lastTime.QuadPart) * 1000 / nFrequency.QuadPart;
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(program);

		manager.Update(deltaTimeMs);
		manager.Draw();
		
		glfwSwapBuffers(pGlfwWindow);

#ifdef _DEBUG
		GLenum error = glGetError();

		while (GL_NO_ERROR != error) {
			throw GetGlError(error);
		}
#endif
	}

	// do shutdown code
	
	Shaders::DestroyProgram(program);
	glfwDestroyWindow(pGlfwWindow);
	Cleanup(hMutex);
	return 0;
}

