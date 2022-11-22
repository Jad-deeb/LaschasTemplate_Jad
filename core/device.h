#pragma once

#include <string>
#include <Windows.h>
#include <D3D11.h>

//! Access to the screen pixels
struct ScreenPixelData
{
  //! Block of memory representing the pixels
  unsigned int* data;
  //! Size in bytes from one row of pixels to the the next row
  int pitch;
};

class Device
{
public:
  Device(HWND hWnd);
  ~Device();
  
  bool present();
  void resize();

  ScreenPixelData* getPixelData();

private:
  bool createDevice();
  void destroyDevice();
  void allocBackBuffer();
  void freeBackBuffer();
  void mapBackBuffer();
  void unmapBackBuffer();

private:
  ID3D11Device* mDxDevice;
  ID3D11DeviceContext* mDxDeviceContext;
  IDXGISwapChain* mDxSwapChain;
  ID3D11Texture2D* mDxBackBufferSwapchainTexture;
  ID3D11Texture2D** mDxBackBufferWriteTexture;
  ScreenPixelData mScreenData;
  INT mCurrBackBufferIndex;
  HWND mHWnd;
  INT mWidth;
  INT mHeight;
  DXGI_FORMAT mBackbufferFormat;
  UINT mBackbufferCount;
  UINT mSwapchainFlags;
};