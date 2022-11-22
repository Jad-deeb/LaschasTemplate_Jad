#pragma once

#include <string>
#include <windows.h>
#include "device.h"

class Window;

//! Identifiers for keyboard buttons
enum KeyId : int
{
  KeyId_Undefined = -1,
  KeyId_None = 0,
  
  // Modifier keys
  //  Note: Modifiers get a bit assigned because they can
  //  also be used as masks to compose the 'WindowKeyModifiers'
  KeyId_Ctrl = 1,
  KeyId_Alt = 2,
  KeyId_Shift = 3,
  
  // Default keys
  KeyId_A, KeyId_B, KeyId_C, KeyId_D, KeyId_E, KeyId_F, KeyId_G, KeyId_H, KeyId_I, KeyId_J, KeyId_K, KeyId_L, KeyId_M, KeyId_N, KeyId_O, KeyId_P, KeyId_Q, KeyId_R, KeyId_S, KeyId_T, KeyId_U, KeyId_V, KeyId_W, KeyId_X, KeyId_Y, KeyId_Z,
  
  // Number keys
  KeyId_0, KeyId_1, KeyId_2, KeyId_3, KeyId_4, KeyId_5, KeyId_6, KeyId_7, KeyId_8, KeyId_9,

  // Function keys
  KeyId_F1, KeyId_F2, KeyId_F3, KeyId_F4, KeyId_F5, KeyId_F6, KeyId_F7, KeyId_F8, KeyId_F9, KeyId_F10, KeyId_F11, KeyId_F12, KeyId_F13, KeyId_F14, KeyId_F15, KeyId_F16, KeyId_F17, KeyId_F18, KeyId_F19, KeyId_F20, KeyId_F21, KeyId_F22, KeyId_F23, KeyId_F24,

  // Arrow keys
  KeyId_Left, KeyId_Right, KeyId_Up, KeyId_Down,
};

typedef KeyId WindowKeyModifiers;

//! Class that describes an abstract window event
struct WindowEvent
{
  //! The window that sends the event.
  Window* window;
};

//! Class that describes an abstract window *input* event
struct WindowInputEvent : public WindowEvent
{
  //! The modifiers that were pressed when the event happened.
  WindowKeyModifiers modifiers;
};

//! Class that describes a key event
struct WindowKeyEvent : public WindowInputEvent
{
  //! The key that the event is about.
  KeyId keyId;
};

//! Callback definition for this event
typedef void(*WindowKeyEventCallback)(const WindowKeyEvent&);



//! The class to create an appication window
class Window
{
public:
  Window(int cWidth, int cHeight, const char* cpTitle = nullptr);
  ~Window();

  //! Runs the window message loop
  //! Returns false in case the window closes
  bool exec();

  //! Push the new frame to the screen
  void present();

  //! Access to certain info about the window
  const char* getTitle() const { return mTitle.c_str(); }
  int getWidth() const { return mWidth; }
  int getHeight() const { return mHeight; }
  void* getHandle() const { return mHWnd; }
  ScreenPixelData* getPixelData();

  //! Set window stuff
  void setTitle(const char* cpTitle);
  void setKeyDownCallback(WindowKeyEventCallback pCallback) { mKeyDownCallback = pCallback; }
  void setKeyUpCallback(WindowKeyEventCallback pCallback) { mKeyUpCallback = pCallback; }

  //! Windows event handlers
  // Note: Only to be called from the WndPrc implementation
  void ProcessClose(UINT cMsg, WPARAM wParam, LPARAM lParam);
  void ProcessMsgKeyDown(UINT cMsg, WPARAM wParam, LPARAM lParam);
  void ProcessMsgKeyUp(UINT cMsg, WPARAM wParam, LPARAM lParam);

private:
  bool createWindow();
  void destroyWindow();

private:
  //! Title of the window
  std::string mTitle;

  //! Dimensions of the window
  int mWidth;
  int mHeight;

  //! Window handle
  HWND mHWnd;
  static int mHWndCount;

  //! Device for graphics
  Device* mDevice;

  //! Event callbacks
  WindowKeyEventCallback mKeyDownCallback;
  WindowKeyEventCallback mKeyUpCallback;
};
