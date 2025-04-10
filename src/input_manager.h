#pragma once

constexpr u32 KEY_COUNT = 256;
constexpr u32 MOUSE_BUTTON_COUNT = 3;

// TODO: factor out DirectInput
//       refactor singleton code
class InputManager
{
private:
     InputManager() {}
     static InputManager instance;
     static bool initialize;
public:
     static void Init();
     static void Terminate();
     static InputManager *Get();
     
     bool KeyDown(u32 key);
     bool KeyJustDown(u32 key);
     bool KeyJustUp(u32 key);
     bool MouseButtonDown(u32 button);
     bool MouseButtonJustDown(u32 button);
     bool MouseButtonJustUp(u32 button);
     i32 MouseX();
     i32 MouseY();
     i32 MouseXMovement();
     i32 MouseYMovement();

     void Process();
     void SetKey(u32 key, bool value);
     void SetMouseButton(u32 button, bool value);
     void SetMousePosition(i32 mouseX, i32 mouseY);
     void SetMouseLastPosition(i32 mouseX, i32 mouseY);
private:
     bool keys[2][KEY_COUNT];
     bool mouseButtons[2][MOUSE_BUTTON_COUNT];
     i32 mousePosX[2];
     i32 mousePosY[2];
     
     IDirectInput8 *directInput;
     IDirectInputDevice8* mouse;
     DIMOUSESTATE2 mouseState;
};

constexpr u32 KEY_ESCAPE = 0x1B;
constexpr u32 KEY_0 = 0x30;
constexpr u32 KEY_1 = 0x31;
constexpr u32 KEY_2 = 0x32;
constexpr u32 KEY_3 = 0x33;
constexpr u32 KEY_4 = 0x34;
constexpr u32 KEY_5 = 0x35;
constexpr u32 KEY_6 = 0x36;
constexpr u32 KEY_7 = 0x37;
constexpr u32 KEY_8 = 0x38;
constexpr u32 KEY_9 = 0x39;
constexpr u32 KEY_A = 0x41;
constexpr u32 KEY_B = 0x42;
constexpr u32 KEY_C = 0x43;
constexpr u32 KEY_D = 0x44;
constexpr u32 KEY_E = 0x45;
constexpr u32 KEY_F = 0x46;
constexpr u32 KEY_G = 0x47;
constexpr u32 KEY_H = 0x48;
constexpr u32 KEY_I = 0x49;
constexpr u32 KEY_J = 0x4A;
constexpr u32 KEY_K = 0x4B;
constexpr u32 KEY_L = 0x4C;
constexpr u32 KEY_M = 0x4D;
constexpr u32 KEY_N = 0x4E;
constexpr u32 KEY_O = 0x4F;
constexpr u32 KEY_P = 0x50;
constexpr u32 KEY_Q = 0x51;
constexpr u32 KEY_R = 0x52;
constexpr u32 KEY_S = 0x53;
constexpr u32 KEY_T = 0x54;
constexpr u32 KEY_U = 0x55;
constexpr u32 KEY_V = 0x56;
constexpr u32 KEY_W = 0x57;
constexpr u32 KEY_X = 0x58;
constexpr u32 KEY_Y = 0x59;
constexpr u32 KEY_Z = 0x5A;
constexpr u32 KEY_NUMPAD0 = 0x60;
constexpr u32 KEY_NUMPAD1 = 0x61;
constexpr u32 KEY_NUMPAD2 = 0x62;
constexpr u32 KEY_NUMPAD3 = 0x63;
constexpr u32 KEY_NUMPAD4 = 0x64;
constexpr u32 KEY_NUMPAD5 = 0x65;
constexpr u32 KEY_NUMPAD6 = 0x66;
constexpr u32 KEY_NUMPAD7 = 0x67;
constexpr u32 KEY_NUMPAD8 = 0x68;
constexpr u32 KEY_NUMPAD9 = 0x69;
constexpr u32 KEY_RETURN = 0x0D;
constexpr u32 KEY_SPACE = 0x20;
constexpr u32 KEY_TAB = 0x09;
constexpr u32 KEY_CONTROL = 0x11;
constexpr u32 KEY_SHIFT = 0x10;
constexpr u32 KEY_ALT = 0x12;
constexpr u32 KEY_CAPS = 0x14;
constexpr u32 KEY_LEFT = 0x25;
constexpr u32 KEY_UP = 0x26;
constexpr u32 KEY_RIGHT = 0x27;
constexpr u32 KEY_DOWN = 0x28;

constexpr u32 MOUSE_BUTTON_LEFT = 0;
constexpr u32 MOUSE_BUTTON_MIDDLE = 1;
constexpr u32 MOUSE_BUTTON_RIGHT = 2;
