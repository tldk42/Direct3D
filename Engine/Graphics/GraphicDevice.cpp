#include "common_pch.h"
#include "GraphicDevice.h"
#include "Debug/Assert.h"
#include "Utils/Math/Color.h"
#include "Window/Window.h"

GraphicDevice::GraphicDevice()
	: mFeatureLevel(D3D_FEATURE_LEVEL_11_0) {}

GraphicDevice::~GraphicDevice()
{
	GraphicDevice::Release();
}

void GraphicDevice::Initialize()
{
	CreateGIFactory();
	CreateDevice();
	CreateSwapChain();

	OnResize(Window::GetWindow()->GetWindowWidth(), Window::GetWindow()->GetWindowHeight());

	Create2DResources();

	Window::GetWindow()->ResizeCallbacks.emplace_back([this](UINT InWidth, UINT InHeight){
		OnResize(InWidth, InHeight);
	});
}

void GraphicDevice::Update(float_t DeltaTime)
{
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
	SetDepthEnable(true);
}

void GraphicDevice::Render()
{
	// 후면 버퍼 렌더
	CheckResult(
		mSwapChain->Present(
			Window::GetWindow()->IsVsyncEnabled(), 0
		));
}

void GraphicDevice::Release()
{
	mDevice             = nullptr;
	mDeviceContext      = nullptr;
	mGIFactory          = nullptr;
	mD2DFactory         = nullptr;
	mDWriteFactory      = nullptr;
	mSwapChain          = nullptr;
	mRenderTargetView   = nullptr;
	mDepthStencilView   = nullptr;
	mDepthStencilState  = nullptr;
	mDepthStencilBuffer = nullptr;
}

void GraphicDevice::ClearColor(const FLinearColor& InColor) const
{
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), InColor.RGBA);
}

void GraphicDevice::SetDepthEnable(bool bEnable) const
{
	mDeviceContext->OMSetDepthStencilState(bEnable ? mDepthStencilState.Get() : nullptr, 1);
}

void GraphicDevice::CreateDevice()
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	CheckResult(
		D3D11CreateDevice(
			nullptr,                  // 주 모니터 사용
			D3D_DRIVER_TYPE_HARDWARE, // 하드웨어 가속 사용
			nullptr,                  // 하드웨어 사용
#ifdef _DEBUG
			D3D11_CREATE_DEVICE_DEBUG | // 디버그 활성화
#endif
			D3D11_CREATE_DEVICE_BGRA_SUPPORT, // flags
			featureLevels,                    // 기능 수준
			ARRAYSIZE(featureLevels),         // 기능 배열 개수
			D3D11_SDK_VERSION,                // DX Version
			mDevice.GetAddressOf(),           // (Out) Device
			&mFeatureLevel,                   // (Out) Features
			mDeviceContext.GetAddressOf()
		));  // (Out) DeviceContext

	if (mFeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		mDeviceContext = nullptr;
		mDevice        = nullptr;
	}
}

void GraphicDevice::CreateGIFactory()
{
	ComPtr<IDXGIAdapter> DXGIAdapter;

	CheckResult(
		CreateDXGIFactory(
			__uuidof(IDXGIFactory),
			reinterpret_cast<void**>(mGIFactory.GetAddressOf()
			)));

	CheckResult(mGIFactory->EnumAdapters(0, DXGIAdapter.GetAddressOf()));

	DXGI_ADAPTER_DESC desc;
	CheckResult(DXGIAdapter->GetDesc(&desc));

	size_t stringLength;
	wcstombs_s(&stringLength, mVideoCardDescription, 128, desc.Description, 128);

	LOG_CORE_INFO("그래픽카드: {}, 메모리: {:d}", mVideoCardDescription, desc.DedicatedVideoMemory / (1 << 20));

	DXGIAdapter = nullptr;

	// Direct2D Creation
	CheckResult(
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, // 멀티 스레드 지원 가능
			mD2DFactory.GetAddressOf()
		));
}

void GraphicDevice::CreateSwapChain()
{
	ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	{
		mSwapChainDesc.BufferCount                        = 1;
		mSwapChainDesc.BufferDesc.Width                   = Window::GetWindow()->GetWindowWidth(); // Buffer Width
		mSwapChainDesc.BufferDesc.Height                  = Window::GetWindow()->GetWindowHeight(); // Buffer Height
		mSwapChainDesc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM; // 색상 출력 형식
		mSwapChainDesc.BufferDesc.RefreshRate.Numerator   = 60; // FPS 분자 TODO: 고정 주사율 설정
		mSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // FPS 분모
		mSwapChainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 버퍼 (렌더링 버퍼)
		mSwapChainDesc.OutputWindow                       = Window::GetWindow()->GetWindowHandle(); // 출력될 윈도우 핸들
		mSwapChainDesc.SampleDesc.Count                   = 1; // 멀티 샘플링 개수
		mSwapChainDesc.SampleDesc.Quality                 = 0; // 멀티 샘플링 품질
		mSwapChainDesc.Windowed                           = !Window::GetWindow()->IsFullScreen(); // 창 전체 화면 모드
		mSwapChainDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD; // Swap이 일어난 이후 버퍼를 Discard
		mSwapChainDesc.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // 적합한 디스플레이로 자동전환
	}

	CheckResult(
		mGIFactory->CreateSwapChain(
			mDevice.Get(),
			&mSwapChainDesc,
			mSwapChain.GetAddressOf()
		));
}


void GraphicDevice::Create2DResources()
{
	// DirectWrite Creation
	CheckResult(
		DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED, // SHARED | ISOLATED타입 (메모리)
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(mDWriteFactory.GetAddressOf())
		));

	CheckResult(
		mRenderTarget_2D->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			mColorBrush.GetAddressOf()
		));
}

void GraphicDevice::SetDepthStencil()
{
	// Depth Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width              = mSwapChainDesc.BufferDesc.Width;
	depthStencilDesc.Height             = mSwapChainDesc.BufferDesc.Height;
	depthStencilDesc.MipLevels          = 1;
	depthStencilDesc.ArraySize          = 1;
	depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags     = 0;
	depthStencilDesc.MiscFlags          = 0;
	CheckResult(
		mDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			mDepthStencilBuffer.GetAddressOf()
		));

	// Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	CheckResult(
		mDevice->CreateDepthStencilView(
			mDepthStencilBuffer.Get(),
			&depthStencilViewDesc,
			mDepthStencilView.GetAddressOf()
		));

	// Depth Stencil State
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
	depthStencilStateDesc.DepthEnable    = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
	CheckResult(
		mDevice->CreateDepthStencilState(
			&depthStencilStateDesc,
			mDepthStencilState.GetAddressOf()
		));
}

void GraphicDevice::SetRenderTarget()
{
	// 3D Side RenderTarget
	ComPtr<ID3D11Texture2D> backBuffer;
	ComPtr<IDXGISurface>    dxgiBackBuffer;

	CheckResult(
		mSwapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf())
		));

	D3D11_RENDER_TARGET_VIEW_DESC desc{};
	{
		desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	}
	CheckResult(
		mDevice->CreateRenderTargetView(
			backBuffer.Get(),
			&desc,
			mRenderTargetView.GetAddressOf()
		));
	mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	// 2D Side RenderTarget (DWrite) 
	CheckResult(
		backBuffer->QueryInterface( // Query Interface
			__uuidof(IDXGISurface), // 내가 원하는 자료형(ID)
			reinterpret_cast<void**>(dxgiBackBuffer.GetAddressOf() // 담아질 개체
			)));

	D2D1_RENDER_TARGET_PROPERTIES props;
	{
		props.type        = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
		props.dpiX        = 96.f;
		props.dpiY        = 96.f;
		props.usage       = D2D1_RENDER_TARGET_USAGE_NONE;
		props.minLevel    = D2D1_FEATURE_LEVEL_DEFAULT;
	}
	CheckResult(
		mD2DFactory->CreateDxgiSurfaceRenderTarget(
			dxgiBackBuffer.Get(),
			&props,
			mRenderTarget_2D.GetAddressOf()
		));

	dxgiBackBuffer = nullptr;
	backBuffer     = nullptr;
}


void GraphicDevice::SetViewportSize(uint32_t InWidth, uint32_t InHeight)
{
	mViewport.Width    = static_cast<float_t>(InWidth);
	mViewport.Height   = static_cast<float_t>(InHeight);
	mViewport.MinDepth = 0.f;
	mViewport.MaxDepth = 1.f;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mDeviceContext->RSSetViewports(1, &mViewport);
}

void GraphicDevice::ResizeSwapChain(uint32_t width, uint32_t height)
{
	CheckResult(
		mSwapChain->ResizeBuffers(
			mSwapChainDesc.BufferCount,
			width, height,
			mSwapChainDesc.BufferDesc.Format,
			mSwapChainDesc.Flags)
	);
	mSwapChain->GetDesc(&mSwapChainDesc);
}

void GraphicDevice::CleanResources()
{
	mDepthStencilState  = nullptr;
	mDepthStencilView   = nullptr;
	mDepthStencilBuffer = nullptr;

	mDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	mRenderTargetView = nullptr;
	mRenderTarget_2D  = nullptr;
}

void GraphicDevice::OnResize(uint32_t InWidth, uint32_t InHeight)
{
	CleanResources();

	ResizeSwapChain(InWidth, InHeight);

	SetDepthStencil();
	SetRenderTarget();
	SetViewportSize(InWidth, InWidth);
}
