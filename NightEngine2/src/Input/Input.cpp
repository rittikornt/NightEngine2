/*!
  @file Input.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Input
*/
#include "Input/Input.hpp"
#include "Graphic/Window.hpp"
#include "Core/Logger.hpp"
#include "UnitTest/UnitTest.hpp"

#include <iostream>
#include "Core/Container/Hashmap.hpp"
#include "Core/Container/Vector.hpp"

using namespace std;
using namespace Core;
using namespace Graphic;

#define EPILSON 0.08f
#define JOYSTICK_MAX 4

namespace Input
{
	/////////////////////////////////////////////////////////////////////////

  //! @brief Array of KeyCode for traversal
#define REGISTER_INPUTKEY(VAR) KeyCode::KEY_##VAR,
	const KeyCode keyCodeEnumArray[]
	{
#include "RegisteredInputKey.inl"
	};
#undef REGISTER_INPUTKEY

  //! @brief Array of KeyCode string
#define STRINGIZE(VAR) #VAR
#define REGISTER_INPUTKEY(VAR) STRINGIZE(KEY_##VAR) ,
	const char* keyCodeName[]
	{
#include "RegisteredInputKey.inl"
	};
#undef REGISTER_INPUTKEY
#undef STRINGIZE

  //! @brief Array of MouseKeyCode for traversal
	const MouseKeyCode mouseKeyCodeEnumArray[]
	{
		MouseKeyCode::MOUSE_BUTTON_LEFT,
		MouseKeyCode::MOUSE_BUTTON_RIGHT,
		MouseKeyCode::MOUSE_BUTTON_MIDDLE
	};

  //! @brief Array of MouseKeyCode string
	const char* mouseKeyCodeName[]
	{
		"MOUSE_BUTTON_LEFT",
		"MOUSE_BUTTON_RIGHT",
		"MOUSE_BUTTON_MIDDLE"
	};

  //! @brief Array of JoyAxisType for traversal
	const JoyAxisType joyAxisEnumArray[]
	{
		JoyAxisType::LeftAxis,
		JoyAxisType::RightAxis,
		JoyAxisType::Trigger
	};

  //! @brief Array of JoyAxisType string
	const char* joyAxisName[]
	{
		"LEFT_AXIS",
		"RIGHT_AXIS",
		"TRIGGER"
	};

  //! @brief Array of JoyButtonCode for traversal
	const JoyButtonCode joyButtonEnumArray[]
	{
		JoyButtonCode::Button_A,
		JoyButtonCode::Button_B,
		JoyButtonCode::Button_X,
		JoyButtonCode::Button_Y,
		JoyButtonCode::Button_LB,
		JoyButtonCode::Button_RB,
		JoyButtonCode::Button_Back,
		JoyButtonCode::Button_Start,
		JoyButtonCode::Button_L3,
		JoyButtonCode::Button_R3,
		JoyButtonCode::Button_Up,
		JoyButtonCode::Button_Right,
		JoyButtonCode::Button_Down,
		JoyButtonCode::Button_Left
	};

  //! @brief Array of JoyButtonCode string
	const char* joyButtonName[]
	{
		"Button_A",
		"Button_B",
		"Button_X",
		"Button_Y",
		"Button_LB",
		"Button_RB",
		"Button_Back",
		"Button_Start",
		"Button_L3",
		"Button_R3",
		"Button_Up",
		"Button_Right",
		"Button_Down",
		"Button_Left"
	};

/////////////////////////////////////////////////////////////////////////

  //**************************************************************
  // Global Variables
  //**************************************************************
	struct EnumClassHash
	{
		template <typename T>
		std::size_t operator()(T t) const
		{
			return static_cast<std::size_t>(t);
		}
	};
	
	//Type Alias
	using KeyStateMap = Container::HashmapCustom<KeyCode, KeyState, EnumClassHash>;
	using JoyButtonStateMap = Container::HashmapCustom<JoyButtonCode, KeyState, EnumClassHash>;
	using MouseInputStateMap = Container::HashmapCustom<MouseKeyCode, KeyState, EnumClassHash>;

	//Data Container
	static KeyStateMap g_keyStateMap;
	static JoyButtonStateMap g_joyButtonStateMap[JOYSTICK_MAX];
	static MouseInputStateMap g_mouseStateMap;

	//Mouse data
	Vector2 g_mousePos;
	Vector2 g_lastMousePos;

  //TODO: MessageObject for toggle debugInput
  bool g_debugInput = false;  

  /////////////////////////////////////////////////////////////////////////

  //**************************************************************
  // Function Definition
  //**************************************************************

	void UpdateKeyState(KeyCode keyCode)
	{
		//Check previous and current state
		int key = static_cast<int>(keyCode);
		KeyState previousState = g_keyStateMap[keyCode];
		bool press = Graphic::Window::GetKeyboardInput(key) == GLFW_PRESS;

		//Result Key State
		KeyState state = KeyState::KEY_NONE;
		if (press)
		{
			// Not pressed previously and start pressing
			if(previousState == KeyState::KEY_NONE
			|| previousState == KeyState::KEY_UP)
			{
				state = KeyState::KEY_DOWN;
			}
			else
			{
				state = KeyState::KEY_HOLD;
			} 
		}
		// Pressed previously and now stop pressing
		else if (previousState == KeyState::KEY_DOWN
			|| previousState == KeyState::KEY_HOLD)
		{
			state = KeyState::KEY_UP;
		}

		g_keyStateMap[keyCode] = state;
	}

	void UpdateKeyState(MouseKeyCode keyCode)
	{
		//Check previous and current state
		int key = static_cast<int>(keyCode);
		KeyState previousState = g_mouseStateMap[keyCode];
		bool press = Graphic::Window::GetMouseInput(key) == GLFW_PRESS;

		//Result Key State
		KeyState state = KeyState::KEY_NONE;
		if (press)
		{
			// Not pressed previously and start pressing
			if (previousState == KeyState::KEY_NONE
				|| previousState == KeyState::KEY_UP)
			{
				state = KeyState::KEY_DOWN;
			}
			else
			{
				state = KeyState::KEY_HOLD;
			}
		}
		// Pressed previously and now stop pressing
		else if (previousState == KeyState::KEY_DOWN
			|| previousState == KeyState::KEY_HOLD)
		{
			state = KeyState::KEY_UP;
		}

		g_mouseStateMap[keyCode] = state;
	}

	void UpdateKeyState(JoyStickNumber joyNumber,JoyButtonCode keyCode)
	{
		//Check previous and current state
		int key = static_cast<int>(keyCode);
    int joyNumIndex = static_cast<int>(joyNumber);
		KeyState previousState = g_joyButtonStateMap[joyNumIndex][keyCode];

		//Get amount of Input count
		int count;
		const unsigned char* button = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

		//No input, just return
		if(count == 0){return;}
		bool press = count > key? button[key] == GLFW_PRESS:false;

		//Result Key State
		KeyState state = KeyState::KEY_NONE;
		if (press)
		{
			// Not pressed previously and start pressing
			if (previousState == KeyState::KEY_NONE
				|| previousState == KeyState::KEY_UP)
			{
				state = KeyState::KEY_DOWN;
			}
			else
			{
				state = KeyState::KEY_HOLD;
			}
		}
		// Pressed previously and now stop pressing
		else if (previousState == KeyState::KEY_DOWN
			|| previousState == KeyState::KEY_HOLD)
		{
			state = KeyState::KEY_UP;
		}

		g_joyButtonStateMap[joyNumIndex][keyCode] = state;
	}

	void UpdateAllInputState()
	{
		//Update all Registered KeyState
		for (auto keyCode : keyCodeEnumArray)
		{
			UpdateKeyState(keyCode);
		}

		//Update all mouse state
		for (auto keyCode : mouseKeyCodeEnumArray)
		{
			UpdateKeyState(keyCode);
		}

		//Update all joy button state
		for (auto keyCode : joyButtonEnumArray)
		{
      UpdateKeyState(JoyStickNumber::Joy_1,keyCode);
		}
	}

	/////////////////////////////////////////////////////////////////////////

	KeyState GetKeyState(KeyCode keyCode)
	{
		return g_keyStateMap[keyCode];
	}

	KeyState GetMouseState(MouseKeyCode keyCode)
	{
		return g_mouseStateMap[keyCode];
	}

	KeyState GetJoyButtonState(JoyStickNumber joyNumber,JoyButtonCode keyCode)
	{
		return g_joyButtonStateMap[static_cast<int>(joyNumber)][keyCode];
	}

	/////////////////////////////////////////////////////////////////////////

	bool GetMouseUp(MouseKeyCode keyCode)
	{
		return GetMouseState(keyCode) == KeyState::KEY_UP;
	}

  bool GetMouseUp(unsigned keyCode)
  {
    return GetMouseUp(static_cast<MouseKeyCode>(keyCode));
  }

	bool GetMouseDown(MouseKeyCode keyCode)
	{
		return GetMouseState(keyCode) == KeyState::KEY_DOWN;
	}

  bool GetMouseDown(unsigned keyCode)
  {
    return GetMouseDown(static_cast<MouseKeyCode>(keyCode));
  }

	bool GetMouseHold(MouseKeyCode keyCode)
	{
		return GetMouseState(keyCode) == KeyState::KEY_HOLD;
	}

  bool GetMouseHold(unsigned keyCode)
  {
    return GetMouseHold(static_cast<MouseKeyCode>(keyCode));
  }

	void Print(glm::vec2 toPrint)
	{
		Debug::Log << "Vec2: " << toPrint.x << ", " << toPrint.y
			<< '\n';
	}

	void Print(glm::vec3 toPrint)
	{
		Debug::Log << "Vec3: " << toPrint.x << ", " << toPrint.y
			<< ", " << toPrint.z << '\n';
	}

	void UpdateMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(Window::GetWindow(), &xpos, &ypos);

		g_lastMousePos = g_mousePos;
		g_mousePos.x = xpos;
		g_mousePos.y = ypos;

		//Print(g_lastMousePos);
		//Print(GetMouseOffset());
	}

	Vector2 GetMousePosition()
	{
		return g_mousePos;
	}

	Vector2 GetMouseOffset()
	{
		//glm::vec2 offset = g_mousePos - g_lastMousePos;
		//offset.y = -offset.y;
		return g_mousePos - g_lastMousePos;
	}

	/////////////////////////////////////////////////////////////////////////

	bool GetKeyUp(KeyCode keyCode)
	{
		return GetKeyState(keyCode) == KeyState::KEY_UP;
	}

  bool GetKeyUp(int keyCode)
  {
    return GetKeyUp(static_cast<KeyCode>(keyCode));
  }

	bool GetKeyDown(KeyCode keyCode)
	{
		return GetKeyState(keyCode) == KeyState::KEY_DOWN;
	}

  bool GetKeyDown(int keyCode)
  {
    return GetKeyDown(static_cast<KeyCode>(keyCode));
  }

	bool GetKeyHold(KeyCode keyCode)
	{
		return GetKeyState(keyCode) == KeyState::KEY_HOLD;
	}

  bool GetKeyHold(int keyCode)
  {
    return GetKeyHold(static_cast<KeyCode>(keyCode));
  }

	/////////////////////////////////////////////////////////////////////////

	inline bool IsValid(float value)
	{
		return value > EPILSON || value < -EPILSON;
	}

	inline bool IsValid(Vector2 value)
	{
		float val = value.x + value.y;
		return val > EPILSON || val < -EPILSON;
	}

	inline bool IsValid(Vector3 value)
	{
		float val = value.x + value.y + value.z;
		return val > EPILSON || val < -EPILSON;
	}

	Vector2 GetJoystickAxis(JoyStickNumber joyNum, JoyAxisType axisType)
	{
		int count;
		const float* axes = glfwGetJoystickAxes(static_cast<int>(joyNum), &count);
		Vector2 result(0.0f,0.0f);

		if (count != 0)
		{
			switch (axisType)
			{
				case JoyAxisType::LeftAxis:
				{
					result.x = IsValid(axes[GLFW_GAMEPAD_AXIS_LEFT_X]) ? axes[GLFW_GAMEPAD_AXIS_LEFT_X] : 0.0f;
					result.y = IsValid(axes[GLFW_GAMEPAD_AXIS_LEFT_Y]) ? -axes[GLFW_GAMEPAD_AXIS_LEFT_Y] : 0.0f;
					break;
				}
				case JoyAxisType::RightAxis:
				{
					result.x = IsValid(axes[GLFW_GAMEPAD_AXIS_RIGHT_X]) ? axes[GLFW_GAMEPAD_AXIS_RIGHT_X] : 0.0f;
					result.y = IsValid(axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]) ? -axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] : 0.0f;
					break;
				}
				case JoyAxisType::Trigger:
				{
					//Init value: -1.0f, upto 1.0f
					result.x = axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > -EPILSON? axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] : 0.0f;
					result.y = axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > -EPILSON? axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] : 0.0f;
					break;
				}
			}
		
		}

		return result;
	}

  Vector2 GetJoystickAxis(unsigned joyNumber, unsigned axisType)
  {
    return GetJoystickAxis(static_cast<JoyStickNumber>(joyNumber), static_cast<JoyAxisType>(axisType));
  }

	bool GetJoyButtonUp(JoyStickNumber joyNumber, JoyButtonCode button)
	{
		return GetJoyButtonState(joyNumber, button) == KeyState::KEY_UP;
	}

  bool GetJoyButtonUp(unsigned joyNumber, unsigned joyButton)
  {
    return GetJoyButtonUp(static_cast<JoyStickNumber>(joyNumber), static_cast<JoyButtonCode>(joyButton));
  }

	bool GetJoyButtonDown(JoyStickNumber joyNumber, JoyButtonCode button)
	{
		return GetJoyButtonState(joyNumber,button) == KeyState::KEY_DOWN;
	}

  bool GetJoyButtonDown(unsigned joyNumber, unsigned joyButton)
  {
    return GetJoyButtonDown(static_cast<JoyStickNumber>(joyNumber), static_cast<JoyButtonCode>(joyButton));
  }

	bool GetJoyButtonHold(JoyStickNumber joyNumber, JoyButtonCode button)
	{
		return GetJoyButtonState(joyNumber,button) == KeyState::KEY_HOLD;
	}

  bool GetJoyButtonHold(unsigned joyNumber, unsigned joyButton)
  {
    return GetJoyButtonHold(static_cast<JoyStickNumber>(joyNumber), static_cast<JoyButtonCode>(joyButton));
  }

	void joystick_callback(int joy, int event)
	{
		int joyNum = joy+1;
		if (event == GLFW_CONNECTED)
		{
				// The joystick was connected
				const char* name = glfwGetJoystickName(joy);

				if(name != nullptr)
				{
					Debug::Log << "Joystick Connected: JoyNumber" << joyNum << "(" << name << ")\n";
				}
		}
		else if (event == GLFW_DISCONNECTED)
		{
				// The joystick was disconnected
				Debug::Log << "Joystick Disconnected: JoyNumber" << joyNum << '\n';
		}
	}
	/////////////////////////////////////////////////////////////////////////

	void DebugPrintInput()
	{
		//Debug all key state
		int index = 0;
		for (auto keyCode : keyCodeEnumArray)
		{
			if (Input::GetKeyUp(keyCode))
			{
				Debug::Log << "GetKeyUp:       " << keyCodeName[index] << '\n';
			}
			if (Input::GetKeyHold(keyCode))
			{
				Debug::Log << "GetKeyHold:     " << keyCodeName[index] << '\n';
			}
			if (Input::GetKeyDown(keyCode))
			{
				Debug::Log << "GetKeyDown:     " << keyCodeName[index] << '\n';
			}

			++index;
		}

		//Debug all mouse state
		index = 0;
		for (auto keyCode : mouseKeyCodeEnumArray)
		{
			if (Input::GetMouseUp(keyCode))
			{
				Debug::Log << "GetMouseUp:       " << mouseKeyCodeName[index] << '\n';
			}
			if (Input::GetMouseHold(keyCode))
			{
				Debug::Log << "GetMouseHold:     " << mouseKeyCodeName[index] << '\n';
			}
			if (Input::GetMouseDown(keyCode))
			{
				Debug::Log << "GetMouseDown:     " << mouseKeyCodeName[index] << '\n';
			}

			++index;
		}

		//Debug all Joystick Input
		for (auto joyButton : joyButtonEnumArray)
		{
			if (Input::GetJoyButtonUp(JoyStickNumber::Joy_1, joyButton))
			{
				Debug::Log << "GetJoyButtonUp:       " << joyButtonName[static_cast<size_t>(joyButton)] << '\n';
			}
			if (Input::GetJoyButtonHold(JoyStickNumber::Joy_1, joyButton))
			{
				Debug::Log << "GetJoyButtonHold:     " << joyButtonName[static_cast<size_t>(joyButton)] << '\n';
			}
			if (Input::GetJoyButtonDown(JoyStickNumber::Joy_1, joyButton))
			{
				Debug::Log << "GetJoyButtonDown:     " << joyButtonName[static_cast<size_t>(joyButton)] << '\n';
			}
		}

		for (auto joyAxis : joyAxisEnumArray)
		{
			Vector2 result = GetJoystickAxis(JoyStickNumber::Joy_1, joyAxis);
			if (IsValid(result))
			{
				Debug::Log << "GetAxis(" << joyAxisName[static_cast<size_t>(joyAxis)]
					<< "): " << result.x << ", " << result.y << '\n';
			}
		}
	}

	void Initialize()
	{
		Debug::Log << "Input::Initialize\n";

		//Set Callback function
		glfwSetJoystickCallback(joystick_callback);
	}

	void OnUpdate()
	{
		//Update all key state for this frame
		UpdateAllInputState();

		//Update mouse pos state
		UpdateMousePosition();

    //Cursor mode Shortcut
    bool ctrl = Input::GetKeyHold(KeyCode::KEY_LEFT_CONTROL);
    if (ctrl)
    {
      if (Input::GetKeyDown(KeyCode::KEY_1))
      {
        Window::SetCursorMode(Window::CursorMode::NORMAL);
      }
      if (Input::GetKeyDown(KeyCode::KEY_2))
      {
        Window::SetCursorMode(Window::CursorMode::DISABLE);
      }
      if (Input::GetKeyDown(KeyCode::KEY_3))
      {
        Window::SetCursorMode(Window::CursorMode::HIDDEN);
      }
    }

		if (Input::GetKeyDown(KeyCode::KEY_F8))
		{
			Container::Vector<char*> argv{ {"Game.exe"}};
			UnitTest::RunTest(argv);
		}
		if (Input::GetKeyDown(KeyCode::KEY_F10))
		{
			Window::ToggleWindowSize();
		}
		if (Input::GetKeyDown(KeyCode::KEY_F11))
		{
			Window::ToggleWindowMode();
		}

		//Debug print input
    if (g_debugInput)
    {
      DebugPrintInput();
    }
	}

	void Terminate()
	{
		Debug::Log << "Input::Terminate\n";
	}
} // Input
