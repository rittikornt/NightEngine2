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
#include "Graphic/Opengl/Window.hpp"

using namespace Core;

namespace Graphic
{
	namespace Window
	{
		//Window states
		GLFWwindow* glfwWindow;
		GLFWmonitor* glfwMonitor;
		unsigned int windowWidth, windowHeight;
		float aspectRatio;
		WindowMode windowMode;

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
			windowWidth = windowResolution[windowResIndex].x;
			windowHeight = windowResolution[windowResIndex].y;

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

			glfwWindow = glfwCreateWindow(windowWidth, windowHeight
				, title, nullptr, nullptr);

			// Check for Valid Context
			if (glfwWindow == nullptr)
			{
				Debug::Log << Logger::MessageType::ERROR_MSG << "Failed to Create OpenGL Context\n";
				fprintf(stderr, "Failed to Create OpenGL Context");
				return;
			}

			Debug::Log << Logger::MessageType::INFO << "Window::Init(), Address: " << glfwWindow << '\n';

			// Create Context and Load OpenGL Functions
			glfwMakeContextCurrent(glfwWindow);
			gladLoadGL();
      //glfwSwapInterval(1);

			Debug::Log << Logger::MessageType::INFO << "OpenGL " << glGetString(GL_VERSION) << '\n';

			//Get monitor
			glfwMonitor = glfwGetPrimaryMonitor();
			SetWindowMode(mode);

			//Set default aspect ratio
			SetWindowAspectRatio(c_DEFAULT_ASPECT_RATIO_X, c_DEFAULT_ASPECT_RATIO_Y);
		
      //Callback when attempted to close window
      glfwSetWindowCloseCallback(glfwWindow, window_close_callback);
    }

		void Terminate()
		{
			glfwDestroyWindow(glfwWindow);
		}

		/////////////////////////////////////////////////////////////////////////

		bool ShouldClose()
		{
			return static_cast<bool>(glfwWindowShouldClose(glfwWindow));
		}

		void SwapBuffer()
		{
			// Flip Buffers and Draw
			glfwSwapBuffers(glfwWindow);
		}

		/////////////////////////////////////////////////////////////////////////

		int GetKeyboardInput(int keyCode)
		{
			return glfwGetKey(glfwWindow, keyCode);
		}

		int GetMouseInput(int mouseKeyCode)
		{
			return glfwGetMouseButton(glfwWindow, mouseKeyCode);
		}

		GLFWwindow * GetWindow(void)
		{
			return glfwWindow;
		}

		int GetWidth()
		{
			return windowWidth;
		}

		int GetHeight()
		{
			return windowHeight;
		}

		/////////////////////////////////////////////////////////////////////////

		void RecenterWindow(void)
		{
			const GLFWvidmode* vidMode = glfwGetVideoMode(glfwMonitor);

			int posX = (abs(float(vidMode->width - (windowWidth))) * 0.15f);
			int posY = (abs(float(vidMode->height - (windowHeight))) * 0.15f);
			glfwSetWindowPos(glfwWindow, posX, posY);

			//Debug::Log << Logger::MessageType::INFO << "vidMode: (" << vidMode->width << ", " << vidMode->height << ")\n";
			Debug::Log << Logger::MessageType::INFO << "Recenter Window to: (" << posX << ", " << posY << ")\n";
		}

		void ToggleWindowSize(void)
		{
			windowResIndex = (windowResIndex + 1) %
				((sizeof(windowResolution) / sizeof(glm::uvec2)));

			SetWindowSize(windowResolution[windowResIndex].x
				, windowResolution[windowResIndex].y);

			RecenterWindow();
		}

		void ToggleWindowMode(void)
		{
			switch (windowMode)
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
			aspectRatio = float(c_DEFAULT_ASPECT_RATIO_X) / c_DEFAULT_ASPECT_RATIO_Y;
			glfwSetWindowAspectRatio(glfwWindow, width, height);
		}

		void SetWindowSize(int width, int height)
		{
			windowWidth = width;
			windowHeight = height;

			glfwSetWindowSize(glfwWindow, width, height);
			glViewport(0, 0, width, height);

			Debug::Log << Logger::MessageType::INFO <<"Window Size: " << width << "x" << height <<'\n';
		}
		
		void SetWindowMode(WindowMode mode)
		{
			windowMode = mode;
			switch (mode)
			{
				case WindowMode::WINDOW:
				{
					glfwSetWindowMonitor(glfwWindow, nullptr
						, 0, 0
						, windowWidth, windowHeight, c_DEFAULT_REFRESHRATE);

					RecenterWindow();
					break;
				}
				case WindowMode::FULLSCREEN:
				{
					const GLFWvidmode* vidMode = glfwGetVideoMode(glfwMonitor);
					glfwSetWindowMonitor(glfwWindow, glfwMonitor
						, 0, 0
						, windowWidth, windowHeight, c_DEFAULT_REFRESHRATE);
					break;
				}
			}
		}

		void SetWindowShouldClose(bool shouldClose)
		{
      Debug::Log << Logger::MessageType::INFO 
        << "SetWindowShouldClose: " << shouldClose;
			glfwSetWindowShouldClose(glfwWindow, shouldClose);

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
					glfwSetInputMode(glfwWindow
						, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					break;
				}
				case CursorMode::DISABLE:
				{
					glfwSetInputMode(glfwWindow
						, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					break;
				}
				case CursorMode::HIDDEN:
				{
					glfwSetInputMode(glfwWindow
						, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					break;
				}
			}
		}

	} // Window 
}	// Graphic 
