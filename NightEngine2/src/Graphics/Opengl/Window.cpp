/*!
  @file Window.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Window
*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Core/Message/MessageObjectList.hpp"
#include "Core/Message/MessageSystem.hpp"
#include "Core/Logger.hpp"

// Standard Headers
#include "Graphics/Opengl/Window.hpp"

using namespace NightEngine;

namespace Rendering
{
	namespace Window
	{
		//Window states
		static GLFWwindow* g_glfwWindow;
		static GLFWmonitor* g_glfwMonitor;
		static unsigned int g_windowWidth, g_windowHeight;
		static float g_aspectRatio;
		static WindowMode g_windowMode;

		size_t windowResIndex = 3;
		const glm::uvec2 windowResolution[] =
		{
			{1024,576},{1280,720},
			{1366,768},{1600,900},
			{1920,1080}
		};

		constexpr int c_DEFAULT_ASPECT_RATIO_X = 16, c_DEFAULT_ASPECT_RATIO_Y = 9;
		constexpr int	c_DEFAULT_REFRESHRATE = 65;

    //********************************************************
    // Callback
    //********************************************************
    void window_close_callback(GLFWwindow* window)
    {
      SetWindowShouldClose(true);
    }

		/////////////////////////////////////////////////////////////////////////

		void Initialize(char* title, WindowMode mode)
		{
			g_windowWidth = windowResolution[windowResIndex].x;
			g_windowHeight = windowResolution[windowResIndex].y;

			// Load GLFW and Create a Window
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
			//Forward Compat for Mac OS
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
      glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

			g_glfwWindow = glfwCreateWindow(g_windowWidth, g_windowHeight
				, title, nullptr, nullptr);

			// Check for Valid Context
			if (g_glfwWindow == nullptr)
			{
				Debug::Log << Logger::MessageType::ERROR_MSG << "Failed to Create OpenGL Context\n";
				fprintf(stderr, "Failed to Create OpenGL Context");
				return;
			}

			Debug::Log << Logger::MessageType::INFO << "Window::Init(), Address: " << g_glfwWindow << '\n';

      //Turn off VSync
      //SetVSync(false);

			// Create Context and Load OpenGL Functions
			glfwMakeContextCurrent(g_glfwWindow);
			gladLoadGL();

			Debug::Log << Logger::MessageType::INFO << "OpenGL " << glGetString(GL_VERSION) << '\n';

			//Get monitor
			g_glfwMonitor = glfwGetPrimaryMonitor();
			SetWindowMode(mode);

			//Set default aspect ratio
			SetWindowAspectRatio(c_DEFAULT_ASPECT_RATIO_X, c_DEFAULT_ASPECT_RATIO_Y);
		
      //Callback when attempted to close window
      glfwSetWindowCloseCallback(g_glfwWindow, window_close_callback);
    }

		void Terminate()
		{
			glfwDestroyWindow(g_glfwWindow);
      glfwTerminate();
		}

		/////////////////////////////////////////////////////////////////////////

		bool ShouldClose()
		{
			return static_cast<bool>(glfwWindowShouldClose(g_glfwWindow));
		}

		void SwapBuffer()
		{
			// Flip Buffers and Draw
			glfwSwapBuffers(g_glfwWindow);
		}

		/////////////////////////////////////////////////////////////////////////

		int GetKeyboardInput(int keyCode)
		{
			return glfwGetKey(g_glfwWindow, keyCode);
		}

		int GetMouseInput(int mouseKeyCode)
		{
			return glfwGetMouseButton(g_glfwWindow, mouseKeyCode);
		}

		GLFWwindow * GetWindow(void)
		{
			return g_glfwWindow;
		}

		int GetWidth()
		{
			return g_windowWidth;
		}

		int GetHeight()
		{
			return g_windowHeight;
		}

		/////////////////////////////////////////////////////////////////////////

		void RecenterWindow(void)
		{
			const GLFWvidmode* vidMode = glfwGetVideoMode(g_glfwMonitor);

			int posX = (abs(float(vidMode->width - (g_windowWidth))) * 0.15f);
			int posY = (abs(float(vidMode->height - (g_windowHeight))) * 0.15f);
			glfwSetWindowPos(g_glfwWindow, posX, posY);

			//Debug::Log << Logger::MessageType::INFO << "vidMode: (" << vidMode->width << ", " << vidMode->height << ")\n";
			Debug::Log << Logger::MessageType::INFO << "Recenter Window to: (" << posX << ", " << posY << ")\n";
		}

		void ToggleWindowSize(void)
		{
      //TODO: Fix bug when changing Window resolution (probably rendertarget size)
			windowResIndex = (windowResIndex + 1) %
				((sizeof(windowResolution) / sizeof(glm::uvec2)));

			SetWindowSize(windowResolution[windowResIndex].x
				, windowResolution[windowResIndex].y);

			RecenterWindow();
		}

		void ToggleWindowMode(void)
		{
			switch (g_windowMode)
			{
				case WindowMode::WINDOW:
				{
					SetWindowMode(WindowMode::FULLSCREEN);
					break;
				}
				case WindowMode::FULLSCREEN:
				{
					SetWindowMode(WindowMode::WINDOW);
					break;
				}
			}
		}

		void SetWindowAspectRatio(int width, int height)
		{
			g_aspectRatio = float(c_DEFAULT_ASPECT_RATIO_X) / c_DEFAULT_ASPECT_RATIO_Y;
			glfwSetWindowAspectRatio(g_glfwWindow, width, height);
		}

		void SetWindowSize(int width, int height)
		{
			g_windowWidth = width;
			g_windowHeight = height;

			glfwSetWindowSize(g_glfwWindow, width, height);
			glViewport(0, 0, width, height);

			Debug::Log << Logger::MessageType::INFO <<"Window Size: " << width << "x" << height <<'\n';
		}
		
		void SetWindowMode(WindowMode mode)
		{
			g_windowMode = mode;
			switch (mode)
			{
				case WindowMode::WINDOW:
				{
					glfwSetWindowMonitor(g_glfwWindow, nullptr
						, 0, 0
						, g_windowWidth, g_windowHeight, c_DEFAULT_REFRESHRATE);

					RecenterWindow();
					break;
				}
				case WindowMode::FULLSCREEN:
				{
					const GLFWvidmode* vidMode = glfwGetVideoMode(g_glfwMonitor);
					glfwSetWindowMonitor(g_glfwWindow, g_glfwMonitor
						, 0, 0
						, g_windowWidth, g_windowHeight, c_DEFAULT_REFRESHRATE);
					break;
				}
			}
		}

		void SetWindowShouldClose(bool shouldClose)
		{
      Debug::Log << Logger::MessageType::INFO 
        << "SetWindowShouldClose: " << shouldClose << "\n";
			glfwSetWindowShouldClose(g_glfwWindow, shouldClose);

			if (shouldClose)
			{
				//++Send Message to System
				GameShouldCloseMessage msg{ true };
				MessageSystem::Get().BroadcastMessage(msg
					, BroadcastScope::HANDLER);
			}
		}

		void SetCursorMode(CursorMode mode)
		{
			switch (mode)
			{
				case CursorMode::NORMAL:
				{
					glfwSetInputMode(g_glfwWindow
						, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					break;
				}
				case CursorMode::DISABLE:
				{
					glfwSetInputMode(g_glfwWindow
						, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					break;
				}
				case CursorMode::HIDDEN:
				{
					glfwSetInputMode(g_glfwWindow
						, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					break;
				}
			}
		}

    void SetVSync(bool enable)
    {
      glfwMakeContextCurrent(g_glfwWindow);
      glfwSwapInterval(enable? 1: 0);
    }

	} // Window 
}	// Rendering 
