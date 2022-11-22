#include "device.h"

#pragma comment(lib, "d3d11.lib")

//! Constructor
Device::Device(HWND hWnd)
  : mDxDevice(NULL)
  , mDxDeviceContext(NULL)
  , mDxSwapChain(NULL)
  , mDxBackBufferSwapchainTexture(NULL)
  , mDxBackBufferWriteTexture(NULL)
  , mCurrBackBufferIndex(0)
  , mHWnd(hWnd)
  , mWidth(0)
  , mHeight(0)
  , mBackbufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
  , mBackbufferCount(2)
  , mSwapchainFlags(0)
{
  if (mBackbufferCount > DXGI_MAX_SWAP_CHAIN_BUFFERS)
    mBackbufferCount = DXGI_MAX_SWAP_CHAIN_BUFFERS;

  mScreenData.data = nullptr;
  mScreenData.pitch = 0;

  createDevice();
}

//! Destructor
Device::~Device()
{
  destroyDevice();
}

//! Create the device object
bool Device::createDevice()
{
  if (mHWnd == NULL)
  {
    printf("Device creation failed because no window was specified\n");
    return false;
  }

  UINT deviceFlags = 0;
  deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL wantedFeatureLevels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };

  // Create the directx device
  RECT wndRect;
  GetClientRect(mHWnd, &wndRect);
  mWidth = wndRect.right - wndRect.left;
  mHeight = wndRect.bottom - wndRect.top;

  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  swapChainDesc.BufferDesc.Width = mWidth;
  swapChainDesc.BufferDesc.Height = mHeight;
  swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
  swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
  swapChainDesc.BufferDesc.Format = mBackbufferFormat;
  swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;
  swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
  swapChainDesc.BufferCount = mBackbufferCount;
  swapChainDesc.OutputWindow = mHWnd;
  swapChainDesc.Windowed = true;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
  swapChainDesc.Flags = mSwapchainFlags;

  HRESULT dxResult = D3D11CreateDeviceAndSwapChain(
    NULL,
    D3D_DRIVER_TYPE_HARDWARE,
    NULL,
    deviceFlags,
    wantedFeatureLevels,
    sizeof(wantedFeatureLevels) / sizeof(D3D_FEATURE_LEVEL),
    D3D11_SDK_VERSION,
    &swapChainDesc,
    &mDxSwapChain,
    &mDxDevice,
    &featureLevel,
    &mDxDeviceContext);

  if FAILED(dxResult)
  {
    printf("D3D11CreateDeviceAndSwapChain() failed with return code %i\n", dxResult);
    return false;
  }

  allocBackBuffer();
  return true;
}

//! Destroy the device objects
void Device::destroyDevice()
{
  freeBackBuffer();

  if (mDxSwapChain != NULL)
  {
    mDxSwapChain->Release();
    mDxSwapChain = NULL;
  }

  if (mDxDeviceContext != NULL)
  {
    mDxDeviceContext->Release();
    mDxDeviceContext = NULL;
  }

  if (mDxDevice != NULL)
  {
    mDxDevice->Release();
    mDxDevice = NULL;
  }
}

//! Retrieve the back-buffer texture from the swap-chain
void Device::allocBackBuffer()
{
  freeBackBuffer();

  // Take texture handle from the backbuffer
  if (mDxSwapChain != NULL)
  {
    // Retrieve backbuffer texture from the swapchain
    mDxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&mDxBackBufferSwapchainTexture);
  
    // Create writable textures
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = (UINT)mWidth;
    textureDesc.Height = (UINT)mHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = mBackbufferFormat;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_STAGING;
    textureDesc.BindFlags = 0;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;

    mDxBackBufferWriteTexture = new ID3D11Texture2D*[mBackbufferCount];

    for (UINT i = 0; i < mBackbufferCount; ++i)
      mDxDevice->CreateTexture2D(&textureDesc, NULL, &mDxBackBufferWriteTexture[i]);
  }

  // Setup initial mapping
  mapBackBuffer();
}

// Return the mapped data of the backbuffer writable texture
void Device::mapBackBuffer()
{
  unmapBackBuffer();

  // Map the current back buffer
  D3D11_MAPPED_SUBRESOURCE mapData;
  HRESULT dxResult = mDxDeviceContext->Map(mDxBackBufferWriteTexture[mCurrBackBufferIndex], 0, D3D11_MAP_WRITE, 0, &mapData);
  if FAILED(dxResult)
  {
    printf("DirectX Backbuffer Map() failed with code: %i\n", dxResult);
    return;
  }

  mScreenData.data = (unsigned int*)mapData.pData;
  mScreenData.pitch = mapData.RowPitch;
}

// Unmap the backbuffer
void Device::unmapBackBuffer()
{
  if (mScreenData.data != nullptr)
    mDxDeviceContext->Unmap(mDxBackBufferWriteTexture[mCurrBackBufferIndex], 0);

  mScreenData.data = nullptr;
  mScreenData.pitch = 0;
}

//! Release the back-buffer texture
void Device::freeBackBuffer()
{
  // Remove any mapping
  unmapBackBuffer();

  // Remove reference to swapchain texture handle to avoid memory leak
  if (mDxBackBufferSwapchainTexture != NULL)
  {
    mDxBackBufferSwapchainTexture->Release();
    mDxBackBufferSwapchainTexture = NULL;
  }

  // Remove references to write texture handle to avoid memory leak
  if (mDxBackBufferWriteTexture != NULL)
  {
    for (UINT i = 0; i < mBackbufferCount; ++i)
      mDxBackBufferWriteTexture[i]->Release();

    delete[] mDxBackBufferWriteTexture;
    mDxBackBufferWriteTexture = NULL;
  }
}

//! Present the backbuffer pixels to the window
bool Device::present()
{
  if (mDxSwapChain != NULL)
  {
    bool result = true;

    // Unmap  the writeable texture so that it can be copied
    unmapBackBuffer();

    //
    mDxDeviceContext->CopyResource(mDxBackBufferSwapchainTexture, mDxBackBufferWriteTexture[mCurrBackBufferIndex]);

    // Present the backbuffer to the screen
    HRESULT dxResult = mDxSwapChain->Present(0, 0);
    if FAILED(dxResult)
    {
      printf("DirectX SwapChain Present() failed with code: %i\n", dxResult);
      result = false;
    }

    // Map the next backbuffer
    mCurrBackBufferIndex++;
    mCurrBackBufferIndex %= mBackbufferCount;

    mapBackBuffer();

    return result;
  }

  return false;
}

//! Resize the swap-chain to match the window size
void Device::resize()
{
  RECT wndRect;
  GetClientRect(mHWnd, &wndRect);

  int width = wndRect.right - wndRect.left;
  int height = wndRect.bottom - wndRect.top;

  if ((width != mWidth) || (height != mHeight))
  {
    freeBackBuffer();

    mWidth = width;
    mHeight = height;
    mDxSwapChain->ResizeBuffers(mBackbufferCount, mWidth, mHeight, mBackbufferFormat, mSwapchainFlags);
    
    allocBackBuffer();
  }
}

//! Returns the screen data
ScreenPixelData* Device::getPixelData()
{
  if (mScreenData.data != nullptr)
    return &mScreenData;
  return nullptr;
}
