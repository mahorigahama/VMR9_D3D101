#include "StdAfx.h"
#include "DGrph.h"

CDGrph::CDGrph() : m_Window(NULL)
{
}

HRESULT CDGrph::CreateDevice(HWND window)
{
	HRESULT hr;
	m_Window = window;
	// 10.1
	DXGI_SWAP_CHAIN_DESC dscd;
	CreateDXGISwapDesc(dscd);
	hr = D3D10CreateDeviceAndSwapChain1(NULL, D3D10_DRIVER_TYPE_HARDWARE,
		NULL, 
#ifdef _DEBUG
		D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
#else
		D3D10_CREATE_DEVICE_BGRA_SUPPORT,
#endif
		D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &dscd,
		&m_SwapChain, &m_Device101);
	if (FAILED(hr)) return hr;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D),
		reinterpret_cast<void **>(&m_BackBuffer));
	// 9
	hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_D3D9Ex);
	if (FAILED(hr)) return hr;
	D3DPRESENT_PARAMETERS d3d9pp;
	CreatePresParam9(d3d9pp);
	hr = m_D3D9Ex->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
			&d3d9pp, NULL, &m_Device9Ex);
	if (FAILED(hr)) return hr;
	// D2D
	D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_D2D1Factory);
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown **>(&m_DWriteFactory));
	return S_OK;
}

void CDGrph::CreateDXGISwapDesc(DXGI_SWAP_CHAIN_DESC &dscd) const
{
	ZeroMemory(&dscd, sizeof(DXGI_SWAP_CHAIN_DESC));
	dscd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dscd.BufferCount = 1;
	dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dscd.OutputWindow = m_Window;
	dscd.SampleDesc.Count = 1;
	dscd.Windowed = TRUE;
}

void CDGrph::CreatePresParam9(D3DPRESENT_PARAMETERS &d3d9pp) const
{
	ZeroMemory(&d3d9pp, sizeof(D3DPRESENT_PARAMETERS));
	d3d9pp.Windowed = TRUE;
	d3d9pp.hDeviceWindow = m_Window;
	d3d9pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3d9pp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3d9pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

void CDGrph::ReleaseResources()
{
	m_D3D9Ex.Release();
	m_Device9Ex.Release();
	m_Device101.Release();
	m_SwapChain.Release();
	m_BackBuffer.Release();
	m_D2D1Factory.Release();
	m_DWriteFactory.Release();
}
