#include "stdafx.h"
#include "DGrph.h"
#include "D3D101VmrAllocator.h"
#include "VMR9_D3D101.h"

namespace {
	const LPCWSTR MOVIE_FILENAME = L"clock.avi";
}

LRESULT CMyVideoWindow::OnCreate(UINT, WPARAM, LPARAM, BOOL&)
{
	if (FAILED(m_DGrph.CreateDevice(m_hWnd)) ||
		FAILED(BuildGraph(MOVIE_FILENAME)))
	{
		DestroyWindow();
	}
	return 0;
}

LRESULT CMyVideoWindow::OnDestroy(UINT, WPARAM, LPARAM, BOOL&)
{
	PostQuitMessage(0);
	return 0;
}

HRESULT CMyVideoWindow::BuildGraph(LPCWSTR filename)
{
	HRESULT hr;
	m_Graph.CoCreateInstance(CLSID_FilterGraph);
	m_Vmr.CoCreateInstance(CLSID_VideoMixingRenderer9);
	CComQIPtr<IVMRFilterConfig9> vmr_config(m_Vmr);
	vmr_config->SetNumberOfStreams(1);
	vmr_config->SetRenderingMode(VMR9Mode_Renderless);
	CComObject<CD3D101VmrAllocator>::CreateInstance(&m_VmrAlloc);
	(*m_VmrAlloc).AddRef();
	m_VmrAlloc->SetDGrph(&m_DGrph);
	CComQIPtr<IVMRSurfaceAllocatorNotify9> vmr_notify(m_Vmr);
	hr = vmr_notify->AdviseSurfaceAllocator(0xacdcacdc, m_VmrAlloc);
	if (FAILED(hr)) return hr;
	hr = m_VmrAlloc->AdviseNotify(vmr_notify);
	if (FAILED(hr)) return hr;
	m_Graph->AddFilter(m_Vmr, L"VMR9");
	hr = m_Graph->RenderFile(filename, NULL);
	if (FAILED(hr)) return hr;
	CComQIPtr<IMediaControl> mc(m_Graph);
	mc->Run();
	return S_OK;
}


bool CVmrTestApp::ParseCommandLine(LPCTSTR lpCmdLine, HRESULT * pnRetCode) throw()
{
	pnRetCode = S_OK;
	return true;
}
HRESULT CVmrTestApp::PreMessageLoop(int nShowCmd) throw()
{
	CRect win_rect;
	win_rect.SetRect(50, 50, 1280 + 50, 720 + 50);
	AdjustWindowRect(win_rect, WS_OVERLAPPEDWINDOW, FALSE);
	m_win.Create(NULL, win_rect, L"VMR9 TEST", WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	m_win.ShowWindow(nShowCmd);
	m_win.UpdateWindow();
	return S_OK;
}

CVmrTestApp _Module;

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	timeBeginPeriod(1);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	_Module.WinMain(nCmdShow);
}
