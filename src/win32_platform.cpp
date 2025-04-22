// tinyxml2
#include "tinyxml2.h"
#include "tinyxml2.cpp"

#include <Windows.h>
#include <Windowsx.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct File
{
     void *data;
     u64 size;
};

// engine
#include "common.h"
#include "memory.h"
#include "memory_manager.h"
#include "containers.h"
#include "graphics_manager.h"
#include "graphics_manager_d3d11.h"
#include "input_manager.h"
#include "notification_manager.h"
#include "model.h"
#include "asset_manager.h"
#include "collision.h"

#include "component.h"
#include "camera_component.h"
#include "actor.h"
#include "systems.h"
#include "physics_system.h"
#include "player_controller.h"
#include "enemy_system.h"
#include "game.h"

void *PlatformGetOsWindow();
void PlaformGetWindowPos(i32 *x, i32 *y);
void PlaformSetCursorPos(i32 x, i32 y);
void PlatformShowMouse(bool show);
void PlatformClientDimensions(i32 *width, i32 *height);
File PlatformReadFile(const char *filepath, i32 stackNum);

// unity build
#include "memory.cpp"
#include "memory_manager.cpp"
#include "containers.cpp"
#include "graphics_manager.cpp"
#include "graphics_manager_d3d11.cpp"
#include "input_manager.cpp"
#include "notification_manager.cpp"
#include "model.cpp"
#include "asset_manager.cpp"
#include "collision.cpp"

#include "camera_component.cpp"

#include "actor.cpp"

#include "physics_system.cpp"
#include "player_controller.cpp"
#include "enemy_system.cpp"
#include "camera_system.cpp"
#include "render_system.cpp"
#include "weapon_system.cpp"

#include "game.cpp"

static bool running;
static bool pause;

static HWND gWindow;
static i32 gWindowX;
static i32 gWindowY;
static i32 gWindowWidth;
static i32 gWindowHeight;

LRESULT Wndproc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
     InputManager* im = InputManager::Get();
     
     LRESULT result = 0;
     
     switch(message)
     {
     case WM_CLOSE:
     {
          running = false;
     }break;
     case WM_SIZE:
     {
          if(wParam == SIZE_MINIMIZED)
          {
               pause = true;
          }
          if(wParam == SIZE_RESTORED)
          {
               pause = false;
          }
          gWindowWidth = LOWORD(lParam);
          gWindowHeight = HIWORD(lParam);
          if(GraphicsManager::Get())
          {
               GraphicsManager::Get()->OnResize(gWindowWidth, gWindowHeight);
          }
          Notification notification = {};
          i32 *size = (i32 *)notification.data;
          size[0] = gWindowWidth;
          size[1] = gWindowHeight;
          NotificationManager::Get()->SendNotification(notification, NOTIFICATION_ON_RESIZE, nullptr);   
     } break;
     
     case WM_MOVE: {
          gWindowX = (int)(short)LOWORD(lParam);
          gWindowY = (int)(short)HIWORD(lParam);
     } break;
     case WM_KEYDOWN:
     case WM_KEYUP:
     {
          DWORD keyCode = (DWORD)wParam;
          bool isDown = ((lParam & (1 << 31)) == 0);
          if (im)
          {
               im->SetKey(keyCode, isDown);
          }
     } break;
     case WM_LBUTTONDOWN:
     case WM_LBUTTONUP:
     case WM_RBUTTONDOWN:
     case WM_RBUTTONUP:
     case WM_MBUTTONDOWN:
     case WM_MBUTTONUP:
     {
          im->SetMouseButton(MOUSE_BUTTON_LEFT, (wParam & MK_LBUTTON) != 0); 
          im->SetMouseButton(MOUSE_BUTTON_MIDDLE, (wParam & MK_MBUTTON) != 0); 
          im->SetMouseButton(MOUSE_BUTTON_RIGHT, (wParam & MK_RBUTTON) != 0); 
     } break;
     
     case WM_MOUSEMOVE:
     {
          i32 mouseX = (i32)GET_X_LPARAM(lParam);
          i32 mouseY = (i32)GET_Y_LPARAM(lParam);
          im->SetMousePosition(mouseX, mouseY);
     } break;     
     default:
     {
          result = DefWindowProc(window, message, wParam, lParam);
     }break;
     }
     
     return result;
}

#if ANT_CONSOLE
int main(int argc, char *argv[])
#else
int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nShowCmd)
#endif
{
#if ANT_CONSOLE
     HINSTANCE hInstance = GetModuleHandle(0);
#endif
     MemoryManager::Init(MB(100), 4);
     InputManager::Init();
     NotificationManager::Init(STATIC_MEMORY);           

     WNDCLASS wndClass = {};
     wndClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
     wndClass.lpfnWndProc = Wndproc;
     wndClass.cbClsExtra = 0;
     wndClass.cbWndExtra = 0;
     wndClass.hInstance = hInstance;
     wndClass.hIcon = 0;
     wndClass.hCursor = 0;
     wndClass.hbrBackground = 0;
     wndClass.lpszMenuName = 0;
     wndClass.lpszClassName = "AntWindowClass";
     if(RegisterClassA(&wndClass))
     {
          HWND window = CreateWindowA(wndClass.lpszClassName,
                                      "Ant", WS_OVERLAPPEDWINDOW,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      0, 0, hInstance, 0);
          if(window)
          {
               gWindow = window;
               
               GraphicsManager::Init((void *)&window, WINDOW_WIDTH, WINDOW_HEIGHT, GraphicsManagerType::D3D11, STATIC_MEMORY);
               ShowWindow(window, SW_SHOW);
               
               Game game;
               game.Init();
               
               running = true;

               LARGE_INTEGER lastCounter, frequency;
               QueryPerformanceFrequency(&frequency);
               QueryPerformanceCounter(&lastCounter);

               SetFocus(window);
               
               MSG message;
               while(running)
               {
                    // Calculates the deltaTime for this frame
                    LARGE_INTEGER currentCounter;
                    QueryPerformanceCounter(&currentCounter);
                    double lastTime = (double)lastCounter.QuadPart / frequency.QuadPart;
                    double currentTime = (double)currentCounter.QuadPart / frequency.QuadPart;
                    lastCounter = currentCounter;
                    float dt = (float)(currentTime - lastTime);
                    if(dt > 0.033f)
                    {
                         dt = 0.033f;
                    }
                    
                    InputManager::Get()->Process();  
                    while(PeekMessageA(&message, window, 0, 0, PM_REMOVE))
                    {
                         TranslateMessage(&message);
                         DispatchMessageA(&message);
                    }
                    if(!pause)
                    {
                         game.Update(dt);
                         game.Render();
                    }    
               }
               game.Terminate();
               GraphicsManager::Terminate();
          }
          else
          {
               // TODO: Logging!
          }
     }
     else
     {
          // TODO: Logging!
     }

     NotificationManager::Terminate();
     InputManager::Terminate();
     MemoryManager::Terminate();

     return 0;
}

void *PlatformGetOsWindow()
{
     return &gWindow;
}

void PlaformGetWindowPos(i32 *x, i32 *y)
{
     *x = gWindowX;
     *y = gWindowY;
}

void PlaformSetCursorPos(i32 x, i32 y)
{
     SetCursorPos(x, y);
}

void PlatformShowMouse(bool show)
{
     ShowCursor(show);
}

void PlatformClientDimensions(i32 *width, i32 *height)
{
     *width = gWindowWidth;
     *height = gWindowHeight;
}

File PlatformReadFile(const char *filepath, i32 stackNum)
{
     HANDLE hFile = CreateFileA(filepath, GENERIC_READ,
                                FILE_SHARE_READ, 0,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL, 0);
     if(hFile == INVALID_HANDLE_VALUE)
     {
          printf("Error: %s failed to be read\n", filepath);
          ASSERT(!"INVALID_CODE_PATH");
     }

     LARGE_INTEGER size;
     GetFileSizeEx(hFile, &size);

     void *data = MemoryManager::Get()->Alloc(size.QuadPart + 1, stackNum);
     memset(data, 0, size.QuadPart + 1);
     DWORD bytesRead = 0;
     if(!ReadFile(hFile, data, size.QuadPart, &bytesRead, 0))
     {
          printf("Error: %s failed to be read\n", filepath);
          ASSERT(!"INVALID_CODE_PATH");  
     }
     
     CloseHandle(hFile);

     File file = {};
     file.data = data;
     file.size = size.QuadPart;

     return file;
}
