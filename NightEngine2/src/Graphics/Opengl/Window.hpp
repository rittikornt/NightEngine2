/*!
  @file Window.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Window
*/
#pragma once

//Forward declaration
struct GLFWwindow;

namespace Rendering
{
	namespace Window
	{
		enum class CursorMode : unsigned char
		{
			NORMAL = 0,
			DISABLE,
			HIDDEN
		};

		enum class WindowMode : unsigned char
		{
			WINDOW = 0,
			FULLSCREEN
		};

		/////////////////////////////////////////////////////////////////////////

		void Initialize(char* title, WindowMode mode);

		void Terminate(void);

		/////////////////////////////////////////////////////////////////////////

		bool ShouldClose(void);

		void SwapBuffer(void);

		/////////////////////////////////////////////////////////////////////////

		int GetKeyboardInput(int keyCode);

		int GetMouseInput(int mouseKeyCode);

		GLFWwindow* GetWindow(void);

		int GetWidth();

		int GetHeight();

		/////////////////////////////////////////////////////////////////////////

		void ToggleWindowSize(void);

		void ToggleWindowMode(void);

		void SetWindowAspectRatio(int width, int height);

		void SetWindowSize(int width, int height);

		void SetWindowMode(WindowMode mode);

		void SetWindowShouldClose(bool shouldClose);

		void SetCursorMode(CursorMode mode);

    void SetVSync(bool enable);
	} // Window 
}	// Rendering 
