#include "window.h"

//! Name of the window class template
#define TEMPLATE_WND_CLASS_NAME "MyWndClassTemplate"

//! Function definition where the Windows OS sends its windows messages
// Note: The function implementation is at the bottom of this file
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//! Function that converts a Windows OS key to our own key ID
// Note: The function implementation is at the bottom of this file
KeyId WndGetWindowKeyId(WPARAM wParam);

//! Declare static members
int Window::mHWndCount = 0;

//! Constructor
Window::Window(int cWidth, int cHeight, const char* cpTitle)
  : mTitle(cpTitle)
  , mWidth(cWidth)
  , mHeight(cHeight)
  , mHWnd(NULL)
  , mDevice(nullptr)
  , mKeyDownCallback(nullptr)
  , mKeyUpCallback(nullptr)
{
  createWindow();
}

//! Destructor
Window::~Window()
{
  destroyWindow();
}

//! Function that creates the window
bool Window::createWindow()
{
  // Get class info to check if this window class is already registered
  WNDCLASSEX wndClass;
  wndClass.cbSize = sizeof(WNDCLASSEX);

  if (!GetClassInfoEx(GetModuleHandle(NULL), TEMPLATE_WND_CLASS_NAME, &wndClass))
  {
    // Register the windows class
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_OWNDC;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = sizeof(this);
    wndClass.hInstance = GetModuleHandle(NULL);
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEMPLATE_WND_CLASS_NAME;
    wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wndClass))
    {
      printf("RegisterClassEx() failed!\n");
      return false;
    }
  }

  mHWnd = CreateWindowEx(
    WS_EX_CLIENTEDGE,
    TEMPLATE_WND_CLASS_NAME,
    mTitle.c_str(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, mWidth, mHeight,
    NULL, NULL, GetModuleHandle(NULL), this);

  if (mHWnd == NULL)
  {
    printf("CreateWindowEx() failed!\n");
    return false;
  }
 
  if (mHWnd != NULL)
  {
    ShowWindow(mHWnd, SW_NORMAL);
  
    // Increase counter to keep track of how many windows have been created
    mHWndCount++;

    // Create the device
    mDevice = new Device(mHWnd);
  }

  return true;
}

//! Function that destroys the window
void Window::destroyWindow()
{
  // Destroy the window
  if (mHWnd != NULL)
  {
    DestroyWindow(mHWnd);
    mHWnd = NULL;
    mHWndCount--;
  }

  // Destroy the device
  if (mDevice != nullptr)
  {
    delete mDevice;
    mDevice = nullptr;
  }

  // Unregister the window class
  if (mHWndCount == 0)
  {
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);

    if (!GetClassInfoEx(GetModuleHandle(NULL), TEMPLATE_WND_CLASS_NAME, &wndClass))
      UnregisterClass(TEMPLATE_WND_CLASS_NAME, GetModuleHandle(NULL));
  }
}

//! Check for window messages
bool Window::exec()
{
  if (mHWnd != NULL)
  {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // Process window events
    while (PeekMessage(&msg, mHWnd, 0, 0, PM_REMOVE) > 0)
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return true;
  }

  return false;
}

//! Tell the device to push the new pixels to the screen
void Window::present()
{
  if (mDevice != nullptr)
    mDevice->present();
}

//! Return the screen pixel data
ScreenPixelData* Window::getPixelData()
{
  if (mDevice != nullptr)
    return mDevice->getPixelData();
  return nullptr;
}

//! Set the title for the window
void Window::setTitle(const char* cpTitle)
{
  if (mHWnd != NULL)
  {
    mTitle = cpTitle;

    if (!SetWindowText(mHWnd, mTitle.c_str()))
      printf("SetWindowText() failed!\n");
  }
}

//! Called by WndPrc when a close message is received
void Window::ProcessClose(UINT cMsg, WPARAM wParam, LPARAM lParam)
{
  destroyWindow();
}

//! Called by WndPrc when a key-down message is received
void Window::ProcessMsgKeyDown(UINT cMsg, WPARAM wParam, LPARAM lParam)
{
  if (mKeyDownCallback != nullptr)
  {
    WindowKeyEvent keyEvent;
    keyEvent.window = this;
    keyEvent.keyId = WndGetWindowKeyId(wParam);
    keyEvent.modifiers = KeyId_None;

    mKeyDownCallback(keyEvent);
  }
}

//! Called by WndPrc when a key-up message is received
void Window::ProcessMsgKeyUp(UINT cMsg, WPARAM wParam, LPARAM lParam)
{
  if (mKeyUpCallback != nullptr)
  {
    WindowKeyEvent keyEvent;
    keyEvent.window = this;
    keyEvent.keyId = WndGetWindowKeyId(wParam);
    keyEvent.modifiers = KeyId_None;

    mKeyUpCallback(keyEvent);
  }
}

// Message proc for the window
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  // Retrieve the window that is mapped to the window handle
  // Note: Available after WM_CREATE message has been received
  Window* window = (Window*)GetWindowLongPtr(hWnd, 0);

  switch (msg)
  {
  case WM_CREATE:
    // Map the window ptr to the window handle
    SetWindowLongPtr(hWnd, 0, (LONG_PTR)(*((Window**)lParam)));
    break;
  case WM_CLOSE:
    window->ProcessClose(msg, wParam, lParam);
    return 0;
  case WM_SIZE:
    break;
  case WM_INPUT:
    break;
  case WM_SYSKEYDOWN:
  case WM_KEYDOWN:
    window->ProcessMsgKeyDown(msg, wParam, lParam);
    return 0;
  case WM_SYSKEYUP:
  case WM_KEYUP:
    window->ProcessMsgKeyUp(msg, wParam, lParam);
    return 0;
  case WM_KILLFOCUS:
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

//! Converts windows virtual key to engine key Id
KeyId WndGetWindowKeyId(WPARAM wParam)
{
  switch (wParam)
  {
  case 'A': return KeyId_A;
  case 'B': return KeyId_B;
  case 'C': return KeyId_C;
  case 'D': return KeyId_D;
  case 'E': return KeyId_E;
  case 'F': return KeyId_F;
  case 'G': return KeyId_G;
  case 'H': return KeyId_H;
  case 'I': return KeyId_I;
  case 'J': return KeyId_J;
  case 'K': return KeyId_K;
  case 'L': return KeyId_L;
  case 'M': return KeyId_M;
  case 'N': return KeyId_N;
  case 'O': return KeyId_O;
  case 'P': return KeyId_P;
  case 'Q': return KeyId_Q;
  case 'R': return KeyId_R;
  case 'S': return KeyId_S;
  case 'T': return KeyId_T;
  case 'U': return KeyId_U;
  case 'V': return KeyId_V;
  case 'W': return KeyId_W;
  case 'X': return KeyId_X;
  case 'Y': return KeyId_Y;
  case 'Z': return KeyId_Z;
  case '0': return KeyId_0;
  case '1': return KeyId_1;
  case '2': return KeyId_2;
  case '3': return KeyId_3;
  case '4': return KeyId_4;
  case '5': return KeyId_5;
  case '6': return KeyId_6;
  case '7': return KeyId_7;
  case '8': return KeyId_8;
  case '9': return KeyId_9;
  case VK_F1: return KeyId_F1;
  case VK_F2: return KeyId_F2;
  case VK_F3: return KeyId_F3;
  case VK_F4: return KeyId_F4;
  case VK_F5: return KeyId_F5;
  case VK_F6: return KeyId_F6;
  case VK_F7: return KeyId_F7;
  case VK_F8: return KeyId_F8;
  case VK_F9: return KeyId_F9;
  case VK_F10: return KeyId_F10;
  case VK_F11: return KeyId_F11;
  case VK_F12: return KeyId_F12;
  case VK_F13: return KeyId_F13;
  case VK_F14: return KeyId_F14;
  case VK_F15: return KeyId_F15;
  case VK_F16: return KeyId_F16;
  case VK_F17: return KeyId_F17;
  case VK_F18: return KeyId_F18;
  case VK_F19: return KeyId_F19;
  case VK_F20: return KeyId_F20;
  case VK_F21: return KeyId_F21;
  case VK_F22: return KeyId_F22;
  case VK_F23: return KeyId_F23;
  case VK_F24: return KeyId_F24;
  case VK_CONTROL: return KeyId_Ctrl;
  case VK_MENU: return KeyId_Alt;
  case VK_SHIFT: return KeyId_Shift;
  case VK_LEFT: return KeyId_Left;
  case VK_RIGHT: return KeyId_Right;
  case VK_UP: return KeyId_Up;
  case VK_DOWN: return KeyId_Down;
  default: return KeyId_Undefined;
  }
}

