/*!
  @file Input.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Input
*/
#pragma once
#include <GLFW/glfw3.h>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace Input
{
	//Type Aliasing
	using Vector2 = glm::vec2;
	using Vector3 = glm::vec3;

#define REGISTER_INPUTKEY(VAR) KEY_##VAR = GLFW_KEY_##VAR,
	enum class KeyCode : int
	{
		KEY_INVALID = -1,
#include "RegisteredInputKey.inl"
		KEY_COUNT
	};
#undef REGISTER_INPUTKEY

	enum class MouseKeyCode : unsigned
	{
		MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
		MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE
	};

	enum class KeyState : unsigned
	{
		KEY_NONE = 0,
		KEY_UP,
		KEY_DOWN,
		KEY_HOLD
	};

	enum class JoyAxisType : unsigned
	{
		LeftAxis = 0,
		RightAxis,
		Trigger
	};

	enum class JoyStickNumber : unsigned
	{
		Joy_1 = GLFW_JOYSTICK_1,
		Joy_2 = GLFW_JOYSTICK_2,
		Joy_3 = GLFW_JOYSTICK_3,
		Joy_4 = GLFW_JOYSTICK_4
	};

	enum class JoyButtonCode : unsigned
	{
		Button_A			= GLFW_GAMEPAD_BUTTON_A,
		Button_B			= GLFW_GAMEPAD_BUTTON_B,
		Button_X			= GLFW_GAMEPAD_BUTTON_X,
		Button_Y			= GLFW_GAMEPAD_BUTTON_Y,
		Button_LB			= GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
		Button_RB			= GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
		Button_Back		= GLFW_GAMEPAD_BUTTON_BACK,
		Button_Start	= GLFW_GAMEPAD_BUTTON_START,
		Button_L3		,
		Button_R3		,
		Button_Up		,
		Button_Right,
		Button_Down	,
		Button_Left
	};
	///////////////////////////////////////////////////////////////////////

	bool GetMouseUp(MouseKeyCode);
  bool GetMouseUp(unsigned);

	bool GetMouseDown(MouseKeyCode);
  bool GetMouseDown(unsigned);

	bool GetMouseHold(MouseKeyCode);
  bool GetMouseHold(unsigned);

	Vector2 GetMousePosition();

	Vector2 GetMouseOffset();

	/////////////////////////////////////////////////////////////////////////

	bool GetKeyUp(KeyCode);
  bool GetKeyUp(int);
  
	bool GetKeyDown(KeyCode);
  bool GetKeyDown(int);

	bool GetKeyHold(KeyCode);
  bool GetKeyHold(int);

	/////////////////////////////////////////////////////////////////////////

	Vector2 GetJoystickAxis(JoyStickNumber, JoyAxisType);
  Vector2 GetJoystickAxis(unsigned joyNumber, unsigned axisType);

	bool GetJoyButtonUp(JoyStickNumber, JoyButtonCode);
  bool GetJoyButtonUp(unsigned joyNumber, unsigned joyButton);

	bool GetJoyButtonDown(JoyStickNumber, JoyButtonCode);
  bool GetJoyButtonDown(unsigned joyNumber, unsigned joyButton);

	bool GetJoyButtonHold(JoyStickNumber, JoyButtonCode);
  bool GetJoyButtonHold(unsigned joyNumber, unsigned joyButton);

	//TODO: Cursor Position

	//TODO: Handle Multiple Joystick Connect/Disconnect Event
	//http://www.glfw.org/docs/latest/input_guide.html#joystick

	/////////////////////////////////////////////////////////////////////////

	void Initialize();

	void OnUpdate();

	void Terminate();
} // Input
